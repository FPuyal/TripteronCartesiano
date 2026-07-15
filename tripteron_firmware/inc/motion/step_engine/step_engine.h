#pragma once

#include "step_engine_interface.h"
#include "tmc_interface.h"

class StepEngine : public IStepEngine {
public:
    StepEngine(const uint16_t threshold, std::shared_ptr<ITmc> xTmc, std::shared_ptr<ITmc> yTmc, std::shared_ptr<ITmc> zTmc) :
        mThreshold(threshold), mXTmc(xTmc), mYTmc(yTmc), mZTmc(zTmc) {}
    ~StepEngine() override = default;

    void SetXSteps(int16_t steps) override;
    void SetYSteps(int16_t steps) override;
    void SetZSteps(int16_t steps) override;
    void RequestUpdate() override { mBufferFlag = true; }
    void Update() override;
    void Tick() override;

private:
    std::shared_ptr<ITmc> mXTmc;
    std::shared_ptr<ITmc> mYTmc;
    std::shared_ptr<ITmc> mZTmc;
    volatile int16_t mSteps[3] = {0, 0, 0};

    // DDS
    const uint16_t mThreshold;
    volatile uint16_t mAccumulator[3] = {0, 0, 0};
    volatile uint8_t mPrevDir[3] = {0, 0, 0};

    static constexpr uint8_t mBufferSize = 40;
    volatile uint8_t mBuffer[mBufferSize] = {};
    volatile uint8_t mBufferIndex = 0;
    volatile bool mBufferFlag = false;
};
