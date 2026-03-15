#pragma once

#include "gpio_output_interface.h"
#include "gpio_wrapper.h"

class GpioOutput : public IGpioOutput, public GpioWrapper {
public:
    GpioOutput(GPIO_TypeDef *gpiox, uint16_t pin) : GpioWrapper(GpioMode::OUTPUT, gpiox, pin) {}
    ~GpioOutput() override = default;

    void Set() override;
    void Reset() override;
    void Toggle() override;
};

