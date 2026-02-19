#pragma once

#include "gpio_manager_interface.h"

#include <map>

class GpioManager : public IGpioManager {
public:
    GpioManager(std::vector<GpioInfo> gpioInfos);
    std::shared_ptr<IGpioWrapper> GetGpio(GpioId id) override;
private:
    bool InitGpios(std::vector<GpioInfo> gpioInfos);
    bool SetGpio(GpioInfo gpioInfo);
    std::map<GpioId, std::shared_ptr<IGpioWrapper>> mGpiosMap;
};



