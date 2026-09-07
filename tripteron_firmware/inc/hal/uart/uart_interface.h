#pragma once

#include "stm32f4xx_hal.h"

#include <memory>
#include <cstdint>

class IUart {
public:
    virtual ~IUart() = default;
    virtual bool WriteData(uint8_t* data, uint8_t len) = 0;
    virtual bool ReadData(uint8_t* data, uint8_t len) = 0;
};

std::shared_ptr<IUart> MakeIUart(UART_HandleTypeDef *huart);
