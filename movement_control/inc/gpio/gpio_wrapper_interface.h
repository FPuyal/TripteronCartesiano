#pragma once

#include <memory>
#include "stm32f407xx.h"

class IGpioWrapper {
public:
    virtual ~IGpioWrapper() = default;
    virtual void Set() = 0;
    virtual void Reset() = 0;
    virtual void Toggle() = 0;
};

std::shared_ptr<IGpioWrapper> MakeIGpioWrapper(GPIO_TypeDef *gpiox, uint16_t pin);
