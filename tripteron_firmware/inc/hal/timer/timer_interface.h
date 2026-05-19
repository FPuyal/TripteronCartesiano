#pragma once

#include "stm32f4xx_hal.h"

#include <memory>

class ITimer {
public:
    virtual ~ITimer() = default;
    virtual void SetFrequency(uint32_t freq) = 0;
};

std::shared_ptr<ITimer> MakeITimer(TIM_HandleTypeDef *htim, uint32_t channel);
