#pragma once

#include "gpio_wrapper_interface.h"

class GpioWrapper : public IGpioWrapper {
public:
    GpioWrapper(GPIO_TypeDef *gpiox, uint16_t pin) : mGPIOx(gpiox), mPin(pin) {}
    void Set() override;
    void Reset() override;
    void Toggle() override;
private:
    GPIO_TypeDef* mGPIOx;
    uint16_t mPin;
};
