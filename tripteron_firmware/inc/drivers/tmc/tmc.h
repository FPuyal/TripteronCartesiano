#pragma once

#include "tmc_interface.h"

#include <memory>

class Tmc : public ITmc {
public:
    Tmc(std::shared_ptr<IGpioOutput> step, std::shared_ptr<IGpioOutput> dir, std::shared_ptr<IGpioOutput> en, std::shared_ptr<IUart> uart, uint8_t nodeAddress, uint16_t microSteps);
    void Enable() override;
    void Disable() override;
    void SetDir(bool dir) override;
    void SetStep(bool step) override;

private:
    void ConfigureRegisters(uint16_t microSteps, uint8_t nodeAddress);

    std::shared_ptr<IGpioOutput> mStep;
    std::shared_ptr<IGpioOutput> mDir;
    std::shared_ptr<IGpioOutput> mEn;
    std::shared_ptr<IUart> mUart;
};
