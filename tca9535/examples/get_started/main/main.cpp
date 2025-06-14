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

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(TCA9535::NewI2cHandle(I2C_NUM_0, GPIO_NUM_18, GPIO_NUM_16, 0x40, &dev_handle));
    TCA9535 tca9535(dev_handle);

    ESP_ERROR_CHECK(tca9535.SetDirection(0xFFFF));          // Set all pins as input
    ESP_ERROR_CHECK(tca9535.SetPolarityInversion(0x0000));  // Set polarity inversion to normal

    uint16_t level;
    while (1) {
        ESP_ERROR_CHECK(tca9535.GetLevel(&level));
        ESP_LOGI(kTag, "Current level: 0x%04X", level);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
