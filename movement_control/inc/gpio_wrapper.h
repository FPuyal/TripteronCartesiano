#pragma once

#include "utils.h"
#include "stm32f407xx.h"

class GPIO {
public:
    GPIO(GPIO_TypeDef *gpiox, uint16_t pin, GPIOId id) : mId(id), mGPIOx(gpiox), mPin(pin) {}
    void Set();
    void Reset();
    void Toggle();

    const GPIOId mId;
private:
    GPIO_TypeDef* mGPIOx;
    uint16_t mPin;
};
