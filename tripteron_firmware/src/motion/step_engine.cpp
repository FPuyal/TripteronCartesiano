#include "step_engine.h"
#include "stm32f407xx.h"

#include <cstdint>
#include <cstdlib>

#include "stm32f4xx_hal_gpio.h"

void StepEngine::Update() {

    uint8_t auxIndex = (mBufferIndex < kBufferSize/2) ? kBufferSize/2 : 0;

    for(int j = 0; j < kBufferSize/2; j++) {
        uint8_t idx = auxIndex + j;
        mBuffer[idx] = 0;
        for (int i = 0; i < 3; i++) {
            const int16_t steps = mSteps[i];
            uint8_t dir = 0;
            uint8_t step = 0;
            mAccumulator[i] += abs(steps);

            if(mAccumulator[i] >= mThreshold) {
                mAccumulator[i] -= mThreshold;
                step = 1;
            }

            dir = (steps > 0) ? 1 : (steps < 0) ? 0 : mPrevDir[i];
            mPrevDir[i] = dir;

            if(step)
                mBuffer[idx] |= (1 << i);
            mBuffer[idx] |= (dir << (i + 3));
        }
    }
}

void StepEngine::Reset() {
    NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);
    NVIC_DisableIRQ(TIM8_UP_TIM13_IRQn);
    NVIC_ClearPendingIRQ(TIM8_UP_TIM13_IRQn);

    for(int i = 0; i < 3; i++) {
        mSteps[i] = 0;
        mAccumulator[i] = 0;
        mPrevDir[i] = 0;
    }

    for(int i = 0; i < kBufferSize; i++)
        mBuffer[i] = 0;

    mBufferIndex = 0;

    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
    NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);

}

void StepEngine::Tick() {
    const uint8_t b = mBuffer[mBufferIndex];

    // 1) STEP a 0
    GPIOE->BSRR = GPIO_PIN_6 << 16;   // X
    GPIOB->BSRR = GPIO_PIN_6 << 16;   // Y
    GPIOD->BSRR = GPIO_PIN_6 << 16;   // Z

    // 2) DIR
    GPIOE->BSRR = (b & 0x08) ? GPIO_PIN_5 : (GPIO_PIN_5 << 16);   // X
    GPIOB->BSRR = (b & 0x10) ? GPIO_PIN_5 : (GPIO_PIN_5 << 16);   // Y
    GPIOD->BSRR = (b & 0x20) ? GPIO_PIN_5 : (GPIO_PIN_5 << 16);   // Z

    mBufferIndex = (mBufferIndex + 1) % kBufferSize;
    if (mBufferIndex == 0 || mBufferIndex == kBufferSize / 2)
        NVIC_SetPendingIRQ(TIM8_UP_TIM13_IRQn);

    // 3) STEP a 1 los que tocan
    if (b & 0x01) GPIOE->BSRR = GPIO_PIN_6;   // X
    if (b & 0x02) GPIOB->BSRR = GPIO_PIN_6;   // Y
    if (b & 0x04) GPIOD->BSRR = GPIO_PIN_6;   // Z

    /* mXTmc->SetDir(b >> 3 & 0x01);
    mYTmc->SetDir(b >> 3 & 0x02);
    mZTmc->SetDir(b >> 3 & 0x04);

    mXTmc->SetStep(b & 0x01);
    mYTmc->SetStep(b & 0x02);
    mZTmc->SetStep(b & 0x04);

    mBufferIndex = (mBufferIndex + 1) % kBufferSize;
    if(mBufferIndex == 0 || mBufferIndex == kBufferSize/2)
        NVIC_SetPendingIRQ(TIM8_UP_TIM13_IRQn); */
}

std::shared_ptr<IStepEngine> MakeIStepEngine(std::shared_ptr<ITmc> xTmc, std::shared_ptr<ITmc> yTmc, std::shared_ptr<ITmc> zTmc, const uint16_t threshold) {
    return std::make_shared<StepEngine>(xTmc, yTmc, zTmc, threshold);
}
