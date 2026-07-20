#include "hardware_manager_interface.h"
#include "step_engine_interface.h"
#include "kinematics_interface.h"

#include "utils.h"

#include <memory>

int main(){

    auto hardwareManager = MakeIHardwareManager();

    hardwareManager->InitHardware();

    extern IStepEngine* stepEngineInstance;
    extern IKinematics* kinematicsInstance;

    auto stepEngine = MakeIStepEngine(
        19200,
        hardwareManager->GetTmc(TmcId::XTmc),
        hardwareManager->GetTmc(TmcId::YTmc),
        hardwareManager->GetTmc(TmcId::ZTmc)
    );
    stepEngineInstance = stepEngine.get();

    auto kinematics = MakeIKinematics(
        hardwareManager->GetEncoder(EncoderId::XEncoder),
        hardwareManager->GetEncoder(EncoderId::YEncoder),
        hardwareManager->GetEncoder(EncoderId::ZEncoder)
    );
    kinematicsInstance = kinematics.get();

    hardwareManager->GetTimer(TimerId::Tim1)->Start();
    hardwareManager->GetTimer(TimerId::Tim2)->Start();

    bool homingFlagX = false;
    bool homingFlagY = false;
    bool homingFlagZ = false;

    stepEngine->SetXSteps(-500);
    while(!homingFlagX) {
        if(hardwareManager->GetEndStop(EndStopId::XEnd)->Read()) {
            stepEngine->SetXSteps(500);
        } else {
            stepEngine->SetXSteps(0);
            homingFlagX = true;
        }
    }

    stepEngine->SetYSteps(-500);
    while(!homingFlagY) {
        if(hardwareManager->GetEndStop(EndStopId::YEnd)->Read()) {
            stepEngine->SetYSteps(500);
        } else {
            stepEngine->SetYSteps(0);
            homingFlagY = true;
        }
    }

    stepEngine->SetZSteps(-500);
    while(!homingFlagZ) {
        if(hardwareManager->GetEndStop(EndStopId::ZEnd)->Read()) {
            stepEngine->SetZSteps(500);
        } else {
            stepEngine->SetZSteps(0);
            homingFlagZ = true;
        }
    }

    hardwareManager->GetTmc(TmcId::XTmc)->Disable();
    hardwareManager->GetTmc(TmcId::YTmc)->Disable();
    hardwareManager->GetTmc(TmcId::ZTmc)->Disable();

    hardwareManager->GetTimer(TimerId::Tim1)->Stop();

    hardwareManager->GetTimer(TimerId::Tim2)->Stop();
    kinematics->CaptureHome();
    hardwareManager->GetTimer(TimerId::Tim2)->Start();

    while(1){
        kinematics->Update();
        KinematicState state = kinematics->GetCurrentState();
    }

}
