#pragma once

#include "hardware_manager_interface.h"
#include "timer_interface.h"
#include <memory>
#include <ostream>

class HardwareManager : public IHardwareManager {
public:
    HardwareManager() = default;
    void InitHardware() override;
    std::shared_ptr<ITimer> GetTimer(TimerId id) override;
    std::shared_ptr<ITmc> GetTmc(TmcId id) override;
    std::shared_ptr<IEncoder> GetEncoder(EncoderId id) override;
    std::shared_ptr<IGpioInput> GetEndStop(EndStopId id) override;
    std::shared_ptr<IUsbCdc> GetUsbCdc() override;

private:
    std::map<TimerId, std::shared_ptr<ITimer>> mTimers;
    std::map<TmcId, std::shared_ptr<ITmc>> mTmcs;
    std::map<EncoderId, std::shared_ptr<IEncoder>> mEncoders;
    std::map<EndStopId, std::shared_ptr<IGpioInput>> mEndStops;
    std::shared_ptr<IUsbCdc> mUsbCdc;
};
