#pragma once

#include "stm32f4xx_hal.h"

#include <memory>
#include <cstdint>

class IGpioInput {
public:
    virtual ~IGpioInput() = default;
    virtual bool Read() = 0;
};

std::shared_ptr<IGpioInput> MakeIGpioInput(GPIO_TypeDef *gpiox, uint16_t pin);
