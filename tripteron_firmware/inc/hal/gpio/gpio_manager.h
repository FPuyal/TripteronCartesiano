#pragma once

#include "gpio_manager_interface.h"
#include "resource_manager.h"
#include "gpio_wrapper_interface.h"

#include <map>

class GpioManager : public ResourceManager<IGpioManager, IGpioWrapper, GpioId, GpioInfo> {
public:
    explicit GpioManager(std::vector<GpioInfo> gpiosInfo) {
        InitResources(gpiosInfo);
    }

    std::shared_ptr<IGpioOutput> GetGpioOutput(GpioId id) override {
        return std::dynamic_pointer_cast<IGpioOutput>(GetResource(id));
    }

    std::shared_ptr<IGpioInput> GetGpioInput(GpioId id) override {
        return std::dynamic_pointer_cast<IGpioInput>(GetResource(id));
    }

protected:
    std::shared_ptr<IGpioWrapper> Make(GpioInfo gpioInfo) override {
        return MakeIGpioWrapper(gpioInfo.mode, gpioInfo.gpiox, gpioInfo.pin);
    }
};
