#pragma once

#include "step_engine_interface.h"
#include "tmc_interface.h"

class StepEngine : public IStepEngine {
public:
    StepEngine(const uint16_t threshold, std::shared_ptr<ITmc> xTmc, std::shared_ptr<ITmc> yTmc, std::shared_ptr<ITmc> zTmc) :
        mThreshold(threshold), mXTmc(xTmc), mYTmc(yTmc), mZTmc(zTmc) {}
    ~StepEngine() override = default;

    void SetSteps(int16_t x, int16_t y, int16_t z) override { mSteps[0] = x; mSteps[1] = y; mSteps[2] = z; };
    void SetXSteps(int16_t x) override { mSteps[0] = x; }
    void SetYSteps(int16_t y) override { mSteps[1] = y; }
    void SetZSteps(int16_t z) override { mSteps[2] = z; }
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

    static constexpr uint8_t kBufferSize = 40;
    volatile uint8_t mBuffer[kBufferSize] = {};
    volatile uint8_t mBufferIndex = 0;
    volatile bool mBufferFlag = false;
};
