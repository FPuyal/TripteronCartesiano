#pragma once

#include "timer_interface.h"
#include "gpio_output_interface.h"
#include "uart_interface.h"

#include <memory>
#include <cstdint>

class ITmc {
public:
    virtual ~ITmc() = default;
    virtual void Enable() = 0;
    virtual void Disable() = 0;
    virtual void SetDir(bool dir) = 0;
    virtual void SetStep(bool step) = 0;
};

std::shared_ptr<ITmc> MakeITmc(std::shared_ptr<IGpioOutput> step, std::shared_ptr<IGpioOutput> dir, std::shared_ptr<IGpioOutput> en, std::shared_ptr<IUart> uart, uint8_t nodeAddress, uint16_t microSteps);
