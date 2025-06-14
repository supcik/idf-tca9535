/**
 ******************************************************************************
 * @file        : main.cpp
 * @brief       : Getting Started App
 * @author      : Jacques Supcik <jacques@supcik.net>
 * @date        : 3 April 2024
 ******************************************************************************
 * @copyright   : Copyright (c) 2024 Jacques Supcik
 * @attention   : SPDX-License-Identifier: MIT
 ******************************************************************************
 * @details
 *
 ******************************************************************************
 */

#include <inttypes.h>
#include <stdio.h>

#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "tca9535.hpp"

static const char* kTag = "main";

extern "C" {
void app_main(void);
}

void app_main(void) {
    ESP_LOGI(kTag, "Starting example");

    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.i2c_port = I2C_NUM_0;
    i2c_mst_config.scl_io_num = GPIO_NUM_16;
    i2c_mst_config.sda_io_num = GPIO_NUM_18;
    i2c_mst_config.glitch_ignore_cnt = 7;
    i2c_mst_config.flags.enable_internal_pullup = false;

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    TCA9535 tca9535(bus_handle, 0x40);

    ESP_ERROR_CHECK(tca9535.SetDirection(0xFFFF));          // Set all pins as input
    ESP_ERROR_CHECK(tca9535.SetPolarityInversion(0x0000));  // Set polarity inversion to normal

    uint16_t level;
    while (1) {
        ESP_ERROR_CHECK(tca9535.GetLevel(&level));
        ESP_LOGI(kTag, "Current level: 0x%04X", level);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
