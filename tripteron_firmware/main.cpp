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

    // --- TEST FSM (sin USB): se inyectan comandos a mano ---
    // 1) Homing: Init -> Homing -> Backoff -> Idle
    CommandRequest homeCmd{};
    homeCmd.stateRequest = StateRequest::Home;
    robot->SetCommandRequest(homeCmd);

    // 2) Move: se prepara ya, pero NO se inyecta hasta que el homing termine
    //    (FSM en Idle). Inyectarlo antes sobrescribiría el Home.
    CommandRequest moveCmd{};
    moveCmd.stateRequest = StateRequest::Move;
    moveCmd.pathSize = 2;
    moveCmd.path[0] = {50.0f, 50.0f, 20.0f};
    moveCmd.path[1] = {100.0f, 80.0f, 30.0f};

    bool movePedido = false;

    while(1){
        robot->Tick();

        // En cuanto el homing termina y la FSM llega a Idle, se lanza el Move una sola vez.
        if(!movePedido && robot->GetState() == State::Idle){
            robot->SetCommandRequest(moveCmd);
            movePedido = true;
        }
    }

}
