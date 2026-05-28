#pragma once

#include "gpio_input_interface.h"
#include "tmc_interface.h"
#include "encoder_interface.h"
#include "utils.h"

#include <memory>
#include <map>

class IHardwareManager {
public:
    virtual ~IHardwareManager() = default;
    virtual void InitHardware() = 0;
    virtual std::shared_ptr<ITmc> GetTmc(TmcId id) = 0;
    virtual std::shared_ptr<IEncoder> GetEncoder(EncoderId id) = 0;
    virtual std::shared_ptr<IGpioInput> GetEndStop(EndStopId id) = 0;
};

std::shared_ptr<IHardwareManager> MakeIHardwareManager();
