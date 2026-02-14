#pragma once

#include <cstdint>
#include <locale>
#include <memory>
#include <vector>
#include <set>
#include "Timer.h"

class TimerManager {
public:
    static TimerManager& GetInstance();
    void InitTimers();
    std::shared_ptr<Timer> GetTimer(TimerId id);
private:
    TimerManager() = default;
    std::vector<std::shared_ptr<Timer>> mTimers;
};



