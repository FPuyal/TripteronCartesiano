#pragma once

#include "stm32f4xx_hal.h"

#include <functional>
#include <memory>

using ReadCallback  = std::function<void(uint16_t raw_value)>;
using ErrorCallback = std::function<void(void)>;

class II2CWrapper {
public:
    virtual ~II2CWrapper() = default;
    virtual bool ReadIT() = 0;
    virtual void OnRxComplete() = 0;
    virtual void OnError() = 0;
    virtual void SetMemAddress(uint8_t addr) = 0;
    virtual void SetReadCallback(ReadCallback callback) = 0;
    virtual void SetErrorCallback(ErrorCallback callback) = 0;
};

std::shared_ptr<II2CWrapper> MakeII2CWrapper(I2C_HandleTypeDef* hi2c, uint16_t devAddress);
