#include "hardware_manager_interface.h"
#include "stm32f4xx_hal.h"
#include "utils.h"

#include "trayectory_profile_interface.h"

#include <memory>

int main(){

    auto hardwareManager = MakeIHardwareManager();

    hardwareManager->InitHardware();

<<<<<<< HEAD
    auto endStopX = hardwareManager->GetEndStop(GpioId::END_STOP_X);

    bool endStopXValue;

    while (1) {
        endStopXValue = endStopX->Read();
=======
    auto tmcX = hardwareManager->GetTmcs()[TmcId::TMCX];
    tmcX->Enable();

    TrajectoryConfig config;
    config.velMax = 5000.0;
    config.accMax = 10000.0;
    config.jerk = 10000.0;

    auto trajProf = MakeITrajectoryProfile(config);

    trajProf->SetTrajectorySegment({0.0, 0.0}, {10000.0, 5000.0});

    while (1) {
        trajProf->Update(0.001f);
        double vel = trajProf->GetVelocity();
        tmcX->SetSpeed(vel);
        HAL_Delay(1);
>>>>>>> 8c847bf (Añadida clase TrayectoryProfile y prueba de concepto del s-curve)
    }

}
