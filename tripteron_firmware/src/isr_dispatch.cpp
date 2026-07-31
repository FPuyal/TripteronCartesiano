#include "step_engine.h"
#include "stm32f407xx.h"

#include "step_engine_interface.h"
#include "kinematics_interface.h"
#include "trajectory_generator_interface.h"
#include "usb_cdc_interface.h"

IStepEngine* stepEngineInstance = nullptr;
IKinematics* kinematicsInstance = nullptr;
ITrajectoryGenerator* trajectoryGeneratorInstance = nullptr;
IUsbCdc* usbCdcInstance = nullptr;

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

extern "C" void UsbCdc_DispatchRx(uint8_t *data, uint16_t len) {
    if(usbCdcInstance) usbCdcInstance->HandleRxEvent(data, len);
}
