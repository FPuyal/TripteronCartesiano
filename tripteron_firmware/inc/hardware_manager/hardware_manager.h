#pragma once

#include "hardware_manager_interface.h"
#include "gpio_manager_interface.h"
#include "timer_manager_interface.h"
#include "tmc_manager_interface.h"

class HardwareManager : public IHardwareManager {
public:
    HardwareManager() = default;
    void InitHardware() override;
    std::map<TmcId, std::shared_ptr<ITmc>> GetTmcs() override;
private:
    std::shared_ptr<IGpioManager> mGpioManager;
    std::shared_ptr<ITimerManager> mTimerManager;
    std::shared_ptr<ITmcManager> mTmcManager;
};
