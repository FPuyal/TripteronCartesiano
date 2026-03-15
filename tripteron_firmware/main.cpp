#include "hardware_manager_interface.h"
#include "utils.h"

#include <memory>

int main(){

    auto hardwareManager = MakeIHardwareManager();

    hardwareManager->InitHardware();

    auto endStopX = hardwareManager->GetEndStop(GpioId::END_STOP_X);

    bool endStopXValue;

    while (1) {
        endStopXValue = endStopX->Read();
    }

}
