#include "hardware_manager_interface.h"
#include "comms_interface.h"

#include "stm32f4xx_hal.h"

#include <memory>

static volatile bool commandPending = false;
static Command lastCommand;

int main(){

    auto hardwareManager = MakeIHardwareManager();
    hardwareManager->InitHardware();

    auto comms = MakeIComms(hardwareManager->GetUsbCdc());

    comms->RegisterCommandCallback([](const Command& command) {
        lastCommand = command;
        commandPending = true;
    });

    while(1){
        if(commandPending){
            commandPending = false;

            for(int i = 0; i < 20; i++){
                float v = static_cast<float>(i);
                TelemetryPayload t{};
                t.measuredPos[0] = v;
                t.measuredPos[1] = v * 2.0f;
                t.measuredPos[2] = v * 3.0f;
                t.measuredVel[0] = v * 0.1f;
                t.measuredVel[1] = v * 0.2f;
                t.measuredVel[2] = v * 0.3f;
                t.executedPos = v;
                t.executedVel = v * 0.5f;
                comms->SendTelemetry(t);
                HAL_Delay(50);
            }

            comms->SendEnd();
        }
    }

}
