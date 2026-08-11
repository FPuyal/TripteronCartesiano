#pragma once

#include "tmc_interface.h"

#include <memory>

class IStepEngine {
public:
    virtual ~IStepEngine() = default;
    virtual void SetSteps(int16_t x, int16_t y, int16_t z) = 0;
    virtual void SetXSteps(int16_t x) = 0;
    virtual void SetYSteps(int16_t y) = 0;
    virtual void SetZSteps(int16_t z) = 0;
    virtual void RequestUpdate() = 0;
    virtual void Update() = 0;
    virtual void Tick() = 0;
};

std::shared_ptr<IStepEngine> MakeIStepEngine(const uint16_t threshold, std::shared_ptr<ITmc> xTmc, std::shared_ptr<ITmc> yTmc, std::shared_ptr<ITmc> zTmc);

