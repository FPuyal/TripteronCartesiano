#pragma once

#include "hardware_manager_interface.h"

#include <memory>
#include <array>
#include <cstddef>

class HardwareManager : public IHardwareManager {
public:
    HardwareManager() = default;
    bool InitHardware() override;
    std::shared_ptr<ITimer> GetTimer(TimerId id) override;
    std::shared_ptr<ITmc> GetTmc(TmcId id) override;
    std::shared_ptr<IEncoder> GetEncoder(EncoderId id) override;
    std::shared_ptr<IGpioInput> GetEndStop(EndStopId id) override;
    std::shared_ptr<IUsbCdc> GetUsbCdc() override;

private:
    std::array<std::shared_ptr<ITimer>, static_cast<size_t>(TimerId::COUNT)> mTimers;
    std::array<std::shared_ptr<ITmc>, static_cast<size_t>(TmcId::COUNT)> mTmcs;
    std::array<std::shared_ptr<IEncoder>, static_cast<size_t>(EncoderId::COUNT)> mEncoders;
    std::array<std::shared_ptr<IGpioInput>, static_cast<size_t>(EndStopId::COUNT)> mEndStops;
    std::shared_ptr<IUsbCdc> mUsbCdc;
};
