#pragma once

#include <memory>
#include <vector>
#include "timer_interface.h"
#include "config.h"

class ITimerManager {
public:
    virtual ~ITimerManager() = default;
    virtual std::shared_ptr<ITimer> GetTimer(TimerId id) = 0;
};

std::shared_ptr<ITimerManager> MakeITimerManager(std::vector<TimerInfo> timersInfos);
