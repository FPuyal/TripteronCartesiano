#pragma once

#include "tmc_interface.h"

#include <memory>

class Tmc : public ITmc {
public:
    Tmc(std::shared_ptr<ITimer> step, std::shared_ptr<IGpioOutput> dir, std::shared_ptr<IGpioOutput> en, std::shared_ptr<IUart> uart, uint16_t microSteps);
    bool Enable() override;
    bool Disable() override;
    void SetDirection(bool dir) override;
    void ToggleDirection() override;
    void SetSpeed(uint32_t freq) override;

private:
    void ConfigureRegisters(uint16_t microSteps);

    std::shared_ptr<ITimer> mStep;
    std::shared_ptr<IGpioOutput> mDir;
    std::shared_ptr<IGpioOutput> mEn;
    std::shared_ptr<IUart> mUart;
};
