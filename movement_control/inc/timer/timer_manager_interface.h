#pragma once

#include "timer_interface.h"
#include "utils.h"

#include <memory>
#include <vector>

class ITimerManager {
public:
    virtual ~ITimerManager() = default;
    virtual std::shared_ptr<ITimer> GetTimer(TimerId id) = 0;
};

std::shared_ptr<ITimerManager> MakeITimerManager(std::vector<TimerInfo> timersInfos);
