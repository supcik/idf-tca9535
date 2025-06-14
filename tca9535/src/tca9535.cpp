/**
 ******************************************************************************
 * @file        : tca9535.cpp
 * @brief       : Driver for the TCA9535 I2C GPIO expander
 * @author      : Jacques Supcik <jacques@supcik.net>
 * @date        : 6 June 2025
 ******************************************************************************
 * @copyright   : Copyright (c) 2025 Jacques Supcik
 * @attention   : SPDX-License-Identifier: MIT
 ******************************************************************************
 * @details
 *
 ******************************************************************************
 */

#include "tca9535.hpp"

#include "esp_err.h"
#include "esp_log.h"
#include "math.h"

static const char* kTag = "TCA9535";
static const int kI2CtimeoutMs = 100;

TCA9535::TCA9535(i2c_master_dev_handle_t handle) {
    handle_ = handle;
    value_ = 0;
}

esp_err_t TCA9535::NewI2cHandle(i2c_port_num_t i2c_port,
                                gpio_num_t sda,
                                gpio_num_t scl,
                                uint16_t dev_addr,
                                i2c_master_dev_handle_t* dev_handle) {
    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.i2c_port = i2c_port;
    i2c_mst_config.scl_io_num = scl;
    i2c_mst_config.sda_io_num = sda;
    i2c_mst_config.glitch_ignore_cnt = 7;
    i2c_mst_config.flags.enable_internal_pullup = false;

    i2c_master_bus_handle_t bus_handle;
    esp_err_t err = i2c_new_master_bus(&i2c_mst_config, &bus_handle);
    if (err != ESP_OK) {
        return err;
    }

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = dev_addr;
    dev_cfg.scl_speed_hz = 100000;

    return i2c_master_bus_add_device(bus_handle, &dev_cfg, dev_handle);
}

esp_err_t TCA9535::SetDirection(uint16_t direction) {
    ESP_LOGD(kTag, "Setting direction: 0x%04X", direction);
    uint8_t buffer[3];
    buffer[0] = 0x06;                     // Register for setting direction
    buffer[1] = (direction >> 8) & 0xFF;  // High byte
    buffer[2] = direction & 0xFF;         // Low byte
    return i2c_master_transmit(this->handle_, buffer, 3, kI2CtimeoutMs);
}
esp_err_t TCA9535::SetPolarityInversion(uint16_t polarity) {
    ESP_LOGD(kTag, "Setting polarity inversion: 0x%04X", polarity);
    uint8_t buffer[3];
    buffer[0] = 0x04;                    // Register for setting polarity inversion
    buffer[1] = (polarity >> 8) & 0xFF;  // High byte
    buffer[2] = polarity & 0xFF;         // Low byte
    return i2c_master_transmit(this->handle_, buffer, 3, kI2CtimeoutMs);
}

esp_err_t TCA9535::SetLevel(uint16_t level) {
    ESP_LOGD(kTag, "Setting level: 0x%04X", level);
    value_ = level;  // Store the level for later use
    uint8_t buffer[3];
    buffer[0] = 0x02;                 // Register for setting output value
    buffer[1] = (level >> 8) & 0xFF;  // High byte
    buffer[2] = level & 0xFF;         // Low byte
    return i2c_master_transmit(this->handle_, buffer, 3, kI2CtimeoutMs);
}

esp_err_t TCA9535::GetLevel(uint16_t* level) {
    ESP_LOGD(kTag, "Getting level");
    uint8_t tx_buffer[1];
    uint8_t rx_buffer[2];
    tx_buffer[0] = 0x00;  // Register for reading input value
    esp_err_t err =
        i2c_master_transmit_receive(this->handle_, tx_buffer, 1, rx_buffer, 2, kI2CtimeoutMs);
    if (err != ESP_OK) {
        return err;
    }
    *level = (rx_buffer[0] << 8) | rx_buffer[1];  // Combine high and low bytes
    ESP_LOGD(kTag, "Read level: 0x%04X", *level);
    return ESP_OK;
}

void TCA9535::SetBit(uint16_t value) {
    ESP_ERROR_CHECK(SetLevel(value_ | value));  // Update the level
}

void TCA9535::ClearBit(uint16_t value) {
    ESP_ERROR_CHECK(SetLevel(value_ & ~value));  // Update the level
}

uint16_t TCA9535::GetValue() {
    uint16_t level;
    ESP_ERROR_CHECK(GetLevel(&level));
    return level;  // Return the current level
}
