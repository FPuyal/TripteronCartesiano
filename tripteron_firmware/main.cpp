#include "hardware_manager_interface.h"
#include "step_engine_interface.h"
#include "kinematics_interface.h"
#include "robot_interface.h"
#include "comms_interface.h"

#include "stm32f4xx_hal.h"
#include "usb_cdc_interface.h"

#include <memory>

extern IStepEngine* stepEngineInstance;
extern IRobot* robotInterface;
extern IKinematics* kinematicsInstance;

int main(){

   auto hardwareManager = MakeIHardwareManager();

    if(!hardwareManager->InitHardware())
        while(1) {}

   auto comms = MakeIComms(hardwareManager->GetUsbCdc());

    auto stepEngine = MakeIStepEngine(
        hardwareManager->GetTmc(TmcId::XTmc),
        hardwareManager->GetTmc(TmcId::YTmc),
        hardwareManager->GetTmc(TmcId::ZTmc),
        19200
    );
    stepEngineInstance = stepEngine.get();

    auto kinematics = MakeIKinematics(
        hardwareManager->GetEncoder(EncoderId::XEncoder),
        hardwareManager->GetEncoder(EncoderId::YEncoder),
        hardwareManager->GetEncoder(EncoderId::ZEncoder)
    );
    kinematicsInstance = kinematics.get();

    auto robot = MakeIRobot(stepEngine,
        hardwareManager->GetEndStop(EndStopId::XEnd),
        hardwareManager->GetEndStop(EndStopId::YEnd),
        hardwareManager->GetEndStop(EndStopId::ZEnd),
        kinematics,
        comms,
        MotionConfig{
            {165.0f, 165.0f, 200.0f},   // posMax (mm)
            {400.0f, 400.0f, 80.0f},    // velMax (mm/s)
            {800.0f, 800.0f, 160.0f},   // accMax (mm/s²)
            0.2f,                       // junctionDeviation (mm)
            {20.0f, 20.0f, 25.0f}}      // stepsPerMm
    );
    robotInterface = robot.get();

    while(1){
        robot->Run();
    }
}
