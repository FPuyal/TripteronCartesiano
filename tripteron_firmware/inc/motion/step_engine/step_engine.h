#pragma once

#include "step_engine_interface.h"

class StepEngine : public IStepEngine {
public:
    StepEngine(const uint16_t threshold, uint8_t* buffer)
        : mThreshold(threshold), mBuffer(buffer) {}
    ~StepEngine() override;

    void Update(int16_t steps[3]) override;

private:
    const uint16_t mThreshold;
    uint16_t mAccumulator[3] = {0, 0, 0};
    uint8_t* mBuffer;
    uint8_t mPrevDir[3] = {0, 0, 0};
};
