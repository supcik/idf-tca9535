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

TCA9535::TCA9535(i2c_master_bus_handle_t bus_handle, uint16_t dev_addr) {
    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = dev_addr;
    dev_cfg.scl_speed_hz = 100000;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &handle_));
    output_reg_ = 0xffff;        // According to datasheet
    polarity_inv_reg_ = 0x0000;  // According to datasheet
    direction_reg_ = 0xffff;     // According to datasheet
}

esp_err_t TCA9535::SetDirection(uint16_t direction) {
    ESP_LOGD(kTag, "Setting direction: 0x%04X", direction);
    direction_reg_ = direction;
    uint8_t buffer[3];
    buffer[0] = 0x06;                     // Register for setting direction
    buffer[1] = direction & 0xFF;         // Low byte
    buffer[2] = (direction >> 8) & 0xFF;  // High byte
    return i2c_master_transmit(this->handle_, buffer, 3, kI2CtimeoutMs);
}

void TCA9535::SetPinsAsOutput(uint16_t mask) {
    ESP_ERROR_CHECK(SetDirection(direction_reg_ & ~(mask)));
}
void TCA9535::SetPinsAsInput(uint16_t mask) {
    ESP_ERROR_CHECK(SetDirection(direction_reg_ | mask));
}

esp_err_t TCA9535::SetPolarityInversion(uint16_t polarity) {
    ESP_LOGD(kTag, "Setting polarity inversion: 0x%04X", polarity);
    polarity_inv_reg_ = polarity;
    uint8_t buffer[3];
    buffer[0] = 0x04;                    // Register for setting polarity inversion
    buffer[1] = polarity & 0xFF;         // Low byte
    buffer[2] = (polarity >> 8) & 0xFF;  // High byte
    return i2c_master_transmit(this->handle_, buffer, 3, kI2CtimeoutMs);
}

void TCA9535::SetPinsPolarityNormal(uint16_t mask) {
    ESP_ERROR_CHECK(SetPolarityInversion(polarity_inv_reg_ & ~(mask)));
}

void TCA9535::SetPinsPolarityInverse(uint16_t mask) {
    ESP_ERROR_CHECK(SetPolarityInversion(polarity_inv_reg_ | mask));
}

esp_err_t TCA9535::SetOutputRegister(uint16_t level) {
    ESP_LOGD(kTag, "Setting level: 0x%04X", level);
    output_reg_ = level;
    uint8_t buffer[3];
    buffer[0] = 0x02;                 // Register for setting output value
    buffer[1] = level & 0xFF;         // Low byte
    buffer[2] = (level >> 8) & 0xFF;  // High byte
    return i2c_master_transmit(this->handle_, buffer, 3, kI2CtimeoutMs);
}

void TCA9535::SetOutputPins(uint16_t mask) {
    ESP_ERROR_CHECK(SetOutputRegister(output_reg_ | mask));  // Update the level
}

void TCA9535::ClearOutputPins(uint16_t mask) {
    ESP_ERROR_CHECK(SetOutputRegister(output_reg_ & ~mask));  // Update the level
}

void TCA9535::SetOutputPinHigh(int pin) {
    ESP_ERROR_CHECK(SetOutputRegister(output_reg_ | (1 << pin)));  // Update the level
}

void TCA9535::SetOutputPinLow(int pin) {
    ESP_ERROR_CHECK(SetOutputRegister(output_reg_ & ~(1 << pin)));  // Update the level
}

void TCA9535::SetOutputPinValue(int pin, bool value) {
    if (value) {
        SetOutputPinHigh(pin);  // Set pin high
    } else {
        SetOutputPinLow(pin);  // Set pin low
    }
}

esp_err_t TCA9535::GetInputRegister(uint16_t* level) {
    ESP_LOGD(kTag, "Getting level");
    uint8_t tx_buffer[1];
    uint8_t rx_buffer[2];
    tx_buffer[0] = 0x00;  // Register for reading input value
    esp_err_t err =
        i2c_master_transmit_receive(this->handle_, tx_buffer, 1, rx_buffer, 2, kI2CtimeoutMs);
    if (err != ESP_OK) {
        return err;
    }
    *level = (rx_buffer[1] << 8) | rx_buffer[0];  // Combine high and low bytes
    ESP_LOGD(kTag, "Read level: 0x%04X", *level);
    return ESP_OK;
}

uint16_t TCA9535::GetInputValue() {
    uint16_t level;
    ESP_ERROR_CHECK(GetInputRegister(&level));
    return level;  // Return the current level
}

bool TCA9535::PinValue(int pin) {
    uint16_t level = GetInputValue();
    return (level & (1 << pin)) != 0;  // Check if the pin is high
}

bool TCA9535::IsPinHigh(int pin) {
    uint16_t level = GetInputValue();
    return (level & (1 << pin)) != 0;  // Check if the pin is high
}

bool TCA9535::IsPinLow(int pin) {
    uint16_t level = GetInputValue();
    return (level & (1 << pin)) == 0;  // Check if the pin is low
}
