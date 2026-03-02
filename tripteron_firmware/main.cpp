#include "hardware_manager_interface.h"
#include "utils.h"

#include <memory>

int main(){

    double angle;
    bool lectura;

    auto hardwareManager = MakeIHardwareManager();

    hardwareManager->InitHardware();

    auto encoders = hardwareManager->GetEncoders();

    encoders[EncoderId::EncoderX]->SetOffset();

    while (1) {

        lectura = encoders[EncoderId::EncoderX]->ReadAngle(angle);

        HAL_Delay(5);
    }

}
