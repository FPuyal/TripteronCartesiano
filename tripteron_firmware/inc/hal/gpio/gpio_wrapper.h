#pragma once

#include "gpio_wrapper_interface.h"

class GpioWrapper : public virtual IGpioWrapper {
public:
    GpioWrapper(GpioMode mode, GPIO_TypeDef *gpiox, uint16_t pin) : mMode(mode), mGPIOx(gpiox), mPin(pin) {}
    GpioMode GetMode() override {return mMode;}

protected:
    GpioMode mMode;
    GPIO_TypeDef* mGPIOx;
    uint16_t mPin;
};
