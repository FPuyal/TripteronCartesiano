#pragma once

#include "stm32f4xx_hal.h"

#include <memory>

class II2CWrapper {
public:
    virtual ~II2CWrapper() = default;
    virtual bool Read(uint16_t& raw_value) = 0;
    virtual bool Write(uint16_t raw_value) = 0;
    virtual void SetMemAddress(uint8_t addr) = 0;
};

std::shared_ptr<II2CWrapper> MakeII2CWrapper(I2C_HandleTypeDef* hi2c, uint16_t devAddress);
