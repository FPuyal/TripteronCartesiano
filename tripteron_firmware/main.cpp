#include "hardware_manager_interface.h"
#include "step_engine_interface.h"
#include "kinematics_interface.h"
#include "robot_interface.h"
#include "comms_interface.h"

#include "usb_cdc_interface.h"
#include "utils.h"

#include <memory>

extern IStepEngine* stepEngineInstance;
extern IRobot* robotInterface;

int main(){

    auto hardwareManager = MakeIHardwareManager();

    hardwareManager->InitHardware();

    auto comms = MakeIComms(hardwareManager->GetUsbCdc());

    auto stepEngine = MakeIStepEngine(
        19200,
        hardwareManager->GetTmc(TmcId::XTmc),
        hardwareManager->GetTmc(TmcId::YTmc),
        hardwareManager->GetTmc(TmcId::ZTmc)
    );
    stepEngineInstance = stepEngine.get();

    auto robot = MakeIRobot(stepEngine,
        hardwareManager->GetEndStop(EndStopId::XEnd),
        hardwareManager->GetEndStop(EndStopId::YEnd),
        hardwareManager->GetEndStop(EndStopId::ZEnd),
        comms
    );
    robotInterface = robot.get();

    hardwareManager->GetTimer(TimerId::Tim1)->Start();
    hardwareManager->GetTimer(TimerId::Tim2)->Start();

    while(1){
        robot->Tick();
    }
}
