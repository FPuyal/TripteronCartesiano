#pragma once

#include "gpio_input_interface.h"

class GpioInput : public IGpioInput {
public:
    GpioInput(GPIO_TypeDef *gpiox, uint16_t pin) : mGPIOx(gpiox), mPin(pin) {}
    ~GpioInput() override = default;

    bool Read() override;

private:
    GPIO_TypeDef* mGPIOx;
    uint16_t mPin;
};


