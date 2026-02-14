#pragma once

#include "stm32f4xx_hal.h"
#include <string>
#include "utils.h"

class Timer {
public:
    Timer(TIM_HandleTypeDef *htim, uint32_t channel) :  mHtim(htim), mChannel(channel) {}
    bool Start();
    bool Stop();
    void SetFrecuency(uint32_t freq);
private:
    TIM_HandleTypeDef *mHtim;
    uint32_t mChannel;
};
