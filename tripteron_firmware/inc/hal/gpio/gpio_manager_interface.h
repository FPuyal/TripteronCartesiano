#pragma once

#include "gpio_output_interface.h"
#include "gpio_input_interface.h"
#include "utils.h"

#include <vector>

class IGpioManager {
public:
    virtual ~IGpioManager() = default;
    virtual std::shared_ptr<IGpioOutput> GetGpioOutput(GpioId id) = 0;
    virtual std::shared_ptr<IGpioInput> GetGpioInput(GpioId id) = 0;
};

std::shared_ptr<IGpioManager> MakeIGpioManager(const std::vector<GpioInfo> gpioInfos);
