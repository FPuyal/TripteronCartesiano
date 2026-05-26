#pragma once

#include "timer_interface.h"

class Timer : public ITimer {
public:
    Timer(TIM_HandleTypeDef *htim, uint32_t channel) :  mHtim(htim), mChannel(channel) {}
    bool SetFrequency(uint32_t freq) override;
private:
    bool Start();
    bool Stop();

    bool mRunning = false;

    TIM_HandleTypeDef *mHtim;
    uint32_t mChannel;
};
