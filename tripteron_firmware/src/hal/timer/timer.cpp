#include "timer.h"

bool Timer::Start(){
    if(HAL_TIM_PWM_Start(mHtim, mChannel) != HAL_OK) return false;
    return true;
}

bool Timer::Stop(){
    if(HAL_TIM_PWM_Stop(mHtim, mChannel) != HAL_OK) return false;
    return true;
}

void Timer::SetFrequency(uint32_t freq){
    if(freq == 0) {
        Stop();
        mRunning = false;
        return;
    }

    if(!mRunning) {
        Start();
        mRunning = true;
    }

    uint32_t timer_clk = HAL_RCC_GetPCLK1Freq();

    // Si el prescaler APB1 > 1, el clock del timer es x2
    if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1)
        timer_clk *= 2;

    uint32_t prescaler = mHtim->Init.Prescaler + 1;
    uint32_t arr = (timer_clk / (prescaler * freq)) - 1;

    __HAL_TIM_SET_AUTORELOAD(mHtim, arr);
    __HAL_TIM_SET_COMPARE(mHtim, mChannel, arr / 2);  // 50%
}

std::shared_ptr<ITimer> MakeITimer(TIM_HandleTypeDef *htim, uint32_t channel) {
    return std::make_shared<Timer>(htim, channel);
}
