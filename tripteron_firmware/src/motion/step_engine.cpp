#include "step_engine.h"
#include "stm32f407xx.h"

#include <cstdint>
#include <cstdlib>

void StepEngine::Update() {
    NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);
    bool flag = mBufferFlag;
    mBufferFlag = false;
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

    if(flag) {
        uint8_t auxIndex = (mBufferIndex < kBufferSize/2) ? kBufferSize/2 : 0;

        for(int j = 0; j < kBufferSize/2; j++) {
            uint8_t idx = auxIndex + j;
            mBuffer[idx] = 0;
            for (int i = 0; i < 3; i++) {
                const int16_t steps = mSteps[i];
                uint8_t dir = steps > 0 ? 1 : 0;
                uint8_t step = 0;
                mAccumulator[i] += abs(steps);

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
    mXTmc->SetDir(mBuffer[mBufferIndex] >> 3 & 0x01);
    mXTmc->SetStep(mBuffer[mBufferIndex] & 0x01);
    mYTmc->SetDir(mBuffer[mBufferIndex] >> 3 & 0x02);
    mYTmc->SetStep(mBuffer[mBufferIndex] & 0x02);
    mZTmc->SetDir(mBuffer[mBufferIndex] >> 3 & 0x04);
    mZTmc->SetStep(mBuffer[mBufferIndex] & 0x04);

    mBufferIndex = (mBufferIndex + 1) % kBufferSize;

    if(mBufferIndex == 0 || mBufferIndex == kBufferSize/2)
        mBufferFlag = true;
}

std::shared_ptr<IStepEngine> MakeIStepEngine(const uint16_t threshold, std::shared_ptr<ITmc> xTmc, std::shared_ptr<ITmc> yTmc, std::shared_ptr<ITmc> zTmc) {
    return std::make_shared<StepEngine>(threshold, xTmc, yTmc, zTmc);
}
