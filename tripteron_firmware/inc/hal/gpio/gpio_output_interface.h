#pragma once

#include "stm32f4xx_hal.h"

#include <memory>

class IGpioOutput {
public:
    virtual ~IGpioOutput() = default;
    virtual void Set() = 0;
    virtual void Reset() = 0;
    virtual void Toggle() = 0;
};

std::shared_ptr<IGpioOutput> MakeIGpioOutput(GPIO_TypeDef *gpiox, uint16_t pin);
