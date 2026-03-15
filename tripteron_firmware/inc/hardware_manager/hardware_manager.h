#pragma once

#include "hardware_manager_interface.h"
#include "encoder_manager_interface.h"
#include "gpio_manager_interface.h"
#include "gpio_input_interface.h"
#include "i2c_manager_interface.h"
#include "timer_manager_interface.h"
#include "tmc_manager_interface.h"
#include <memory>

class HardwareManager : public IHardwareManager {
public:
    HardwareManager() = default;
    void InitHardware() override;
    std::map<TmcId, std::shared_ptr<ITmc>> GetTmcs() override;
    std::map<EncoderId, std::shared_ptr<IEncoder>> GetEncoders() override;
    std::shared_ptr<IGpioInput> GetEndStop(GpioId id) override;
private:
    std::shared_ptr<IGpioManager> mGpioManager;
    std::shared_ptr<ITimerManager> mTimerManager;
    std::shared_ptr<ITmcManager> mTmcManager;
    std::shared_ptr<II2CManager> mI2CManager;
    std::shared_ptr<IEncoderManager> mEncoderManager;
};
