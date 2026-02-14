#pragma once

#include "utils.h"
#include "stm32f407xx.h"

class GPIO {
public:
    GPIO(GPIO_TypeDef *gpiox, uint16_t pin) : mGPIOx(gpiox), mPin(pin) {}
    void Set();
    void Reset();
    void Toggle();
private:
    GPIO_TypeDef* mGPIOx;
    uint16_t mPin;
};
