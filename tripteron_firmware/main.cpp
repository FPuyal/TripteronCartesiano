#include "hardware_manager_interface.h"
#include "step_engine_interface.h"
#include "kinematics_interface.h"
#include "trajectory_generator_interface.h"

#include "utils.h"

#include <memory>

int main(){

    auto hardwareManager = MakeIHardwareManager();

    hardwareManager->InitHardware();

    extern IStepEngine* stepEngineInstance;
    extern IKinematics* kinematicsInstance;
    extern ITrajectoryGenerator* trajectoryGeneratorInstance;


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

    auto trajectoryGenerator = MakeITrajectoryGenerator(TrajectoryConfig {
        240.0,
        240.0
    });

    trajectoryGeneratorInstance = trajectoryGenerator.get();

    trajectoryGenerator->SetTrajectoryProfile(
        MotionState { 0.0, 0.0 },
        MotionState { 150.0, 0.0 }
    );

    while(1){
        trajectoryGenerator->Update();
        stepEngine->SetXSteps(trajectoryGenerator->GetVelocity() * trajectoryGenerator->GetDirection() * 20.0);
    }

}
