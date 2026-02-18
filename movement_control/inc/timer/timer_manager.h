#pragma once

#include <map>
#include "timer_manager_interface.h"

class TimerManager : public ITimerManager {
public:
    TimerManager(std::vector<TimerInfo> timersInfos);
    std::shared_ptr<ITimer> GetTimer(TimerId id) override;
private:
    bool InitTimers(std::vector<TimerInfo> timersInfos);
    bool SetTimer(TimerInfo timerInfo);
    std::map<TimerId, std::shared_ptr<ITimer>> mTimersMap;
};
