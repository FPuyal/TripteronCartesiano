#include "step_engine.h"

void StepEngine::Update(int16_t steps[3]) {
    for (int i = 0; i < 3; i++) {
        uint8_t dir = steps[i] > 0 ? 1 : 0;

        if (dir != mPrevDir[i]) {
            mBuffer[i] = 0;          // no step en el tick del flip
            mBuffer[i + 3] = dir;
            mPrevDir[i] = dir;
            continue;               // o saltar la lógica de acumulador este tick
        }

        mAccumulator[i] += abs(steps[i]);
        if(mAccumulator[i] >= mThreshold) {
            mAccumulator[i] -= mThreshold;
            mBuffer[i] = 1;
        } else
            mBuffer[i] = 0;
    }
}
