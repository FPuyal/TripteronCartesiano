#include "hardware_manager_interface.h"
#include "stm32f4xx_hal.h"
#include "axis_interface.h"
#include "axis.h"
#include "trajectory_generator_utils.h"
#include "utils.h"

#include <memory>
#include <vector>


int main(){

    auto hardwareManager = MakeIHardwareManager();

    hardwareManager->InitHardware();

    auto XAxis = MakeIAxis(
        hardwareManager->GetEndStop(GpioId::END_STOP_X),
        hardwareManager->GetTmc(TmcId::TMCX),
        AxisConfig{10, 201, 300, 1000});

    std::vector<MotionState> segment = {{10, 10}, {20, 20}, {30, 30}, {40, 40}, {50, 50}};

    XAxis->RequestState(AxisCommandRequest::Enable);
    XAxis->Tick();
    HAL_Delay(1);
    XAxis->Tick();
    HAL_Delay(1);

    auto state = XAxis->GetState();

    XAxis->RequestState(AxisCommandRequest::Home);
    XAxis->Tick();
    HAL_Delay(1);
    state = XAxis->GetState();

    while (XAxis->GetState() != AxisState::Idle) {
        XAxis->Tick();
        HAL_Delay(1);
    }

   bool segmentSet = XAxis->SetSegment(segment);

    XAxis->RequestState(AxisCommandRequest::Move);
    XAxis->Tick();
    HAL_Delay(1);
    state = XAxis->GetState();

    while (1) {
        XAxis->Tick();
        HAL_Delay(1);
    }
}

