#include "step_engine.h"
#include "stm32f407xx.h"

#include "step_engine_interface.h"
#include "kinematics_interface.h"
#include "trajectory_generator_interface.h"

IStepEngine* stepEngineInstance = nullptr;
IKinematics* kinematicsInstance = nullptr;
ITrajectoryGenerator* trajectoryGeneratorInstance = nullptr;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        if(stepEngineInstance) stepEngineInstance->Tick();
    }
    if(htim->Instance == TIM2) {
        if(trajectoryGeneratorInstance) trajectoryGeneratorInstance->RequestUpdate();
        if(stepEngineInstance) stepEngineInstance->Update();
        if(kinematicsInstance) kinematicsInstance->RequestUpdate();

    }
}
