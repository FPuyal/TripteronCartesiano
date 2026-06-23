#include "step_engine.h"

void StepEngine::SetSteps(int16_t steps[3]) {
    mSteps[0] = steps[0];
    mSteps[1] = steps[1];
    mSteps[2] = steps[2];
}

void StepEngine::Update() {
    // Generar tantos tick como sean necesario para su consumo.
    if(mBufferFlag) {
        mBufferFlag = false;

        uint8_t auxIndex = (mBufferIndex < mBufferSize/2) ? mBufferSize/2 : 0;

        for(int j = 0; j < mBufferSize/2; j++) {
            uint8_t idx = auxIndex + j;
            mBuffer[idx] = 0;
            for (int i = 0; i < 3; i++) {
                uint8_t dir = mSteps[i] > 0 ? 1 : 0;
                uint8_t step = 0;
                mAccumulator[i] += abs(mSteps[i]);

                if(mAccumulator[i] >= mThreshold) {
                    mAccumulator[i] -= mThreshold;
                    step = 1;
                }

                if (dir != mPrevDir[i]) {
                    step = 0;
                    mPrevDir[i] = dir;
                }
                if(step)
                    mBuffer[idx] |= (1 << i);
                mBuffer[idx] |= (dir << (i + 3));
            }
        }
    }
}

void StepEngine::Tick() {
    // Prueba para el TMCX
    mTmc->SetDir(mBuffer[mBufferIndex] >> 3 & 0x01);
    mTmc->SetStep(mBuffer[mBufferIndex] & 0x01);

    mBufferIndex = (mBufferIndex + 1) % mBufferSize;

    if(mBufferIndex == 0 || mBufferIndex == mBufferSize/2)
        mBufferFlag = true;
}

std::unique_ptr<IStepEngine> MakeIStepEngine(const uint16_t threshold, ITmc& xTmc) {
    return std::make_unique<StepEngine>(threshold, xTmc);
}
