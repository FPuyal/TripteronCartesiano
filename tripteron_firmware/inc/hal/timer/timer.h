#pragma once

#include "timer_interface.h"

class Timer : public ITimer {
public:
    Timer(TIM_HandleTypeDef *htim) :  mHtim(htim) {}
    bool Start() override;
    bool Stop() override;
    bool IsRunning() const { return mRunning; }

private:
    TIM_HandleTypeDef *mHtim;
    bool mRunning = false;
};
