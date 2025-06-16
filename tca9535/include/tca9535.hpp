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
    void SetPinsAsOutput(uint16_t mask);
    void SetPinsAsInput(uint16_t mask);

    esp_err_t SetPolarityInversion(uint16_t polarity);
    void SetPinsPolarityNormal(uint16_t mask);
    void SetPinsPolarityInverse(uint16_t mask);

    esp_err_t SetOutputRegister(uint16_t level);
    void SetOutputPins(uint16_t mask);
    void ClearOutputPins(uint16_t mask);
    void SetOutputPinHigh(int pin);
    void SetOutputPinLow(int pin);
    void SetOutputPinValue(int pin, bool value);

    esp_err_t GetInputRegister(uint16_t* level);
    uint16_t GetInputValue();

    bool PinValue(int pin);
    bool IsPinHigh(int pin);
    bool IsPinLow(int pin);

   private:
    i2c_master_dev_handle_t handle_;
    uint16_t output_reg_ = 0x0ff;        // According to datasheet
    uint16_t polarity_inv_reg_ = 0x000;  // According to datasheet
    uint16_t direction_reg_ = 0x0ff;     // According to datasheet
};
