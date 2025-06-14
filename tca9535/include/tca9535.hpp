/**
 ******************************************************************************
 * @file        : tca9535.hpp
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

#pragma once

#include "driver/i2c_master.h"

class TCA9535 {
   public:
    TCA9535(i2c_master_bus_handle_t bus_handle, uint16_t dev_addr);

    esp_err_t SetDirection(uint16_t direction);
    esp_err_t SetPolarityInversion(uint16_t polarity);

    esp_err_t SetLevel(uint16_t level);
    esp_err_t GetLevel(uint16_t* level);

    void SetBit(uint16_t value);
    void ClearBit(uint16_t value);
    uint16_t GetValue();

   private:
    i2c_master_dev_handle_t handle_;
    uint16_t value_;
};
