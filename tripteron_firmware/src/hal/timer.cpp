#include "timer.h"

bool Timer::Start(){
    mRunning = true;
    __HAL_TIM_CLEAR_FLAG(mHtim, TIM_FLAG_UPDATE);
    return HAL_TIM_Base_Start_IT(mHtim) == HAL_OK;
}

bool Timer::Stop(){
    mRunning = false;
    return HAL_TIM_Base_Stop_IT(mHtim) == HAL_OK;
}

std::shared_ptr<ITimer> MakeITimer(TIM_HandleTypeDef *htim) {
    return std::make_shared<Timer>(htim);
}
