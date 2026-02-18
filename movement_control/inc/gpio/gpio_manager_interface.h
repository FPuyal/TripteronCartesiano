#pragma once

#include <vector>
#include "gpio_wrapper_interface.h"
#include "config.h"

class IGpioManager {
public:
    virtual ~IGpioManager() = default;
    virtual std::shared_ptr<IGpioWrapper> GetGpio(GpioId id) = 0;
};

std::shared_ptr<IGpioManager> MakeIGpioManager(const std::vector<GpioInfo> gpioInfos);
