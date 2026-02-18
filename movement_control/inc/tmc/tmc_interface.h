#pragma once

#include <memory>
#include "timer_interface.h"
#include "gpio_Wrapper_interface.h"

class ITmc {
public:
    virtual ~ITmc() = default;
    virtual bool Enable() = 0;
    virtual bool Disable() = 0;
    virtual void SetDirection(bool dir) = 0;
    virtual void ToggleDirection() = 0;
    virtual void SetSpeed(uint32_t freq) = 0;
};

std::shared_ptr<ITmc> MakeITmc(std::shared_ptr<ITimer> step, std::shared_ptr<IGpioWrapper> dir, std::shared_ptr<IGpioWrapper> en);
