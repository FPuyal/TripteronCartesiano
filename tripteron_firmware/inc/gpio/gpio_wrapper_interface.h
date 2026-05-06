#pragma once

#include "utils.h"

#include "stm32f4xx.h"
#include <memory>

class IGpioWrapper {
public:
    virtual ~IGpioWrapper() = default;
    virtual GpioMode GetMode() = 0;
};

std::shared_ptr<IGpioWrapper> MakeIGpioWrapper(GpioMode mode, GPIO_TypeDef *gpiox, uint16_t pin);
