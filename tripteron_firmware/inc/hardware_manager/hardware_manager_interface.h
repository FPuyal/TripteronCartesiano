#pragma once

#include "timer_interface.h"
#include "gpio_input_interface.h"
#include "tmc_interface.h"
#include "encoder_interface.h"
#include "usb_cdc_interface.h"
#include "hardware_manager_types.h"

#include <memory>

class IHardwareManager {
public:
    virtual ~IHardwareManager() = default;
    virtual void InitHardware() = 0;
    virtual std::shared_ptr<ITimer> GetTimer(TimerId id) = 0;
    virtual std::shared_ptr<ITmc> GetTmc(TmcId id) = 0;
    virtual std::shared_ptr<IEncoder> GetEncoder(EncoderId id) = 0;
    virtual std::shared_ptr<IGpioInput> GetEndStop(EndStopId id) = 0;
    virtual std::shared_ptr<IUsbCdc> GetUsbCdc() = 0;
};

std::shared_ptr<IHardwareManager> MakeIHardwareManager();
