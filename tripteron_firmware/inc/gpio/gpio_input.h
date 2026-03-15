#pragma once

#include "gpio_input_interface.h"
#include "gpio_wrapper.h"

class GpioInput : public IGpioInput, public GpioWrapper {
public:
    GpioInput(GPIO_TypeDef *gpiox, uint16_t pin) : GpioWrapper(GpioMode::INPUT, gpiox, pin) {}
    ~GpioInput() override = default;

    bool Read() override;
};


