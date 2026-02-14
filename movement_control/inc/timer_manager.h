#pragma once

#include <cstdint>
#include <locale>
#include <map>
#include <memory>
#include <set>
#include "Timer.h"

class TimerManager {
public:
    static TimerManager& GetInstance();
    bool InitTimers();
    std::shared_ptr<Timer> GetTimer(TimerId id);
private:
    TimerManager() = default;
    bool AddTimer(TimerId id, TIM_HandleTypeDef* htim, uint16_t channel);
    std::map<TimerId, std::shared_ptr<Timer>> mTimersMap;
};



