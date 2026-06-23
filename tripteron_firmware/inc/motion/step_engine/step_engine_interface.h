#pragma once

#include "tmc_interface.h"

#include <memory>

class IStepEngine {
public:
    virtual ~IStepEngine() = default;
    virtual void SetSteps(int16_t steps[3]) = 0;
    virtual void Update() = 0;
    virtual void Tick() = 0;
};

std::unique_ptr<IStepEngine> MakeIStepEngine(const uint16_t threshold, ITmc& xTmc);

