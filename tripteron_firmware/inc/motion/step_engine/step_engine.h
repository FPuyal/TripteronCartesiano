#pragma once

#include "step_engine_interface.h"
#include "tmc_interface.h"

class StepEngine : public IStepEngine {
public:
    StepEngine(const uint16_t threshold, ITmc& xTmc) : mThreshold(threshold), mTmc{&xTmc} {}
    ~StepEngine() override = default;

    void SetSteps(int16_t steps[3]) override;
    void Update() override;
    void Tick() override;

private:
    ITmc* mTmc;
    int16_t mSteps[3] = {0, 0, 0};

    // DDS
    const uint16_t mThreshold;
    uint16_t mAccumulator[3] = {0, 0, 0};
    uint8_t mPrevDir[3] = {0, 0, 0};

    static constexpr uint8_t mBufferSize = 20;
    uint8_t mBuffer[mBufferSize] = {};
    volatile uint8_t mBufferIndex = 0;
    volatile bool mBufferFlag = false;
};
