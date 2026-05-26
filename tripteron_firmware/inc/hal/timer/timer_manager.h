#pragma once

#include "timer_manager_interface.h"
#include "resource_manager.h"

class TimerManager : public ResourceManager<ITimerManager, ITimer, TimerId, TimerInfo> {
public:
    TimerManager(std::vector<TimerInfo> timersInfo) : ResourceManager() {
        InitResources(timersInfo);
    }
    std::shared_ptr<ITimer> GetTimer(TimerId id) override {
        return GetResource(id);
    }
protected:
    std::shared_ptr<ITimer> Make(TimerInfo timerInfo) override {
        return MakeITimer(timerInfo.htim, timerInfo.channel);
    }
};
