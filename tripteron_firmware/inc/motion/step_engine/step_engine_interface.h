#pragma once

#include "tmc_interface.h"

#include <memory>

class IStepEngine {
public:
    virtual ~IStepEngine() = default;
    virtual void SetXSteps(int16_t steps) = 0;
    virtual void SetYSteps(int16_t steps) = 0;
    virtual void SetZSteps(int16_t steps) = 0;
    virtual void RequestUpdate() = 0;
    virtual void Update() = 0;
    virtual void Tick() = 0;
};

std::unique_ptr<IStepEngine> MakeIStepEngine(const uint16_t threshold, std::shared_ptr<ITmc> xTmc, std::shared_ptr<ITmc> yTmc, std::shared_ptr<ITmc> zTmc);

