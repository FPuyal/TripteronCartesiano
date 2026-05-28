#pragma once

#include "gpio_output_interface.h"

class GpioOutput : public IGpioOutput {
public:
    GpioOutput(GPIO_TypeDef *gpiox, uint16_t pin) : mGPIOx(gpiox), mPin(pin) {}
    ~GpioOutput() override = default;

    void Set() override;
    void Reset() override;
    void Toggle() override;

private:
    GPIO_TypeDef* mGPIOx;
    uint16_t mPin;
};

