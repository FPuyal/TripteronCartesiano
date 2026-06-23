#include "step_engine.h"
#include "stm32f401xe.h"

#include "step_engine_interface.h"

IStepEngine* stepEngineInstance = nullptr;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if(stepEngineInstance) stepEngineInstance->Tick();
    }
}
