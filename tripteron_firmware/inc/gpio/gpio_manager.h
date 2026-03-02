#pragma once

#include "gpio_manager_interface.h"
#include "resource_manager.h"

#include <map>

class GpioManager : public ResourceManager<IGpioManager, IGpioWrapper, GpioId, GpioInfo> {
public:
    GpioManager(std::vector<GpioInfo> gpiosInfo) : ResourceManager() {
        InitResources(gpiosInfo);
    }
    std::shared_ptr<IGpioWrapper> GetGpio(GpioId id) override {
        return GetResource(id);
    }
protected:
    std::shared_ptr<IGpioWrapper> Make(GpioInfo gpioInfo) override {
        return MakeIGpioWrapper(gpioInfo.gpiox, gpioInfo.pin);
    }
};
