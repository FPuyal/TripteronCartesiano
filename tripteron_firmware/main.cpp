#include "hardware_manager_interface.h"
#include "step_engine_interface.h"
#include "kinematics_interface.h"
#include "motion_controller_interface.h"

#include "utils.h"

#include <memory>

extern IMotionController* motionControllerInstance;
extern IStepEngine* stepEngineInstance;

int main(){

    auto hardwareManager = MakeIHardwareManager();

    hardwareManager->InitHardware();

    hardwareManager->GetTmc(TmcId::XTmc)->Enable();
    hardwareManager->GetTmc(TmcId::YTmc)->Enable();

    auto stepEngine = MakeIStepEngine(
        19200,
        hardwareManager->GetTmc(TmcId::XTmc),
        hardwareManager->GetTmc(TmcId::YTmc),
        hardwareManager->GetTmc(TmcId::ZTmc)
    );
    stepEngineInstance = stepEngine.get();

    auto motionController = MakeIMotionController(
        stepEngine,
        MotionConfig{
            .posMax = {200.0f, 200.0f, 200.0f},
            .velMax = {240.0f, 240.0f, 48.0f},
            .velMin = {30.0f, 30.0f, 30.0f},
            .accMax = {480.0f, 480.0f, 96.0f},
            .stepsPerMm = {20.0f, 20.0f, 25.0f}
        }
    );
    motionControllerInstance = motionController.get();

    hardwareManager->GetTimer(TimerId::Tim1)->Start();
    hardwareManager->GetTimer(TimerId::Tim2)->Start();

    bool homingFlagX = false;
    bool homingFlagY = false;
    bool homingFlagZ = false;

    while(!homingFlagX) {
        if(!hardwareManager->GetEndStop(EndStopId::XEnd)->Read()) {
            stepEngine->SetXSteps(-500);
        } else {
            stepEngine->SetXSteps(0);
            homingFlagX = true;
        }
    }

    while(!homingFlagY) {
        if(!hardwareManager->GetEndStop(EndStopId::YEnd)->Read()) {
            stepEngine->SetYSteps(-500);
        } else {
            stepEngine->SetYSteps(0);
            homingFlagY = true;
        }
    }

    while(!homingFlagZ) {
        if(!hardwareManager->GetEndStop(EndStopId::ZEnd)->Read()) {
            stepEngine->SetZSteps(-500);
        } else {
            stepEngine->SetZSteps(0);
            homingFlagZ = true;
        }
    }

    MotionData segments[] = {
        {20.0f, 20.0f, 20.0f},
        {150.0f, 20.0f, 20.0f},
        {150.0f, 150.0f, 20.0f},
        {20.0f, 150.0f, 20.0f},
        {20.0f, 20.0f, 20.0f}
    };

    motionController->SetSegments(segments, sizeof(segments) / sizeof(segments[0]));

    while(1){
        motionController->Move();
        motionController->Update();
    }

}
