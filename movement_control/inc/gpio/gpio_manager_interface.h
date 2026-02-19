#pragma once

#include "gpio_wrapper_interface.h"
#include "utils.h"

#include <vector>

class IGpioManager {
public:
    virtual ~IGpioManager() = default;
    virtual std::shared_ptr<IGpioWrapper> GetGpio(GpioId id) = 0;
};

std::shared_ptr<IGpioManager> MakeIGpioManager(const std::vector<GpioInfo> gpioInfos);
