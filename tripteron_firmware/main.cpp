#include "hardware_manager_interface.h"
#include "stm32f4xx_hal.h"
#include "trajectory_generator_utils.h"
#include "utils.h"

#include "trajectory_generator_interface.h"

#include <memory>

#include "usart.h"

int main(){

    auto hardwareManager = MakeIHardwareManager();

    hardwareManager->InitHardware();

    auto tmcX = hardwareManager->GetTmcs()[TmcId::TMCX];
    tmcX->Enable();

    TrajectoryConfig config {
        12000,
        1600
    };

    auto trajectoryGenerator = MakeITrajectoryGenerator(config);

    MotionState initState {0.0, 0.0};
    MotionState finalState {100000, config.velMax};

    trajectoryGenerator->SetTrajectoryProfile(initState, finalState);

    while (1) {
        while (!trajectoryGenerator->IsFinished()) {
            trajectoryGenerator->Update(0.001); // Update every 10 ms

            double velocity = trajectoryGenerator->GetVelocity();

            tmcX->SetSpeed(static_cast<uint32_t>(velocity));

            HAL_Delay(1); // Delay for 10 ms
        }
    }
}
