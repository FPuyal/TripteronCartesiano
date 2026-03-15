#pragma once

#include "gpio_wrapper_interface.h"

class IGpioInput : public virtual IGpioWrapper {
public:
    virtual ~IGpioInput() = default;
    virtual bool Read() = 0;
};

std::shared_ptr<IGpioInput> MakeIGpioInput(GPIO_TypeDef *gpiox, uint16_t pin);
