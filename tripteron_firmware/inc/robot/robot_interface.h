#pragma once

#include "motion_controller_interface.h"
#include "step_engine_interface.h"
#include "gpio_input_interface.h"
#include "comms_interface.h"

#include "robot_types.h"

#include <memory>

class IRobot {
public:
    virtual void Run() = 0;
    virtual void Tick() = 0;
    virtual void SetCommandRequest(CommandRequest commandRequest) = 0;
    virtual void EmergencyStop() = 0;
    virtual State GetState() const = 0;
};

std::shared_ptr<IRobot> MakeIRobot(std::shared_ptr<IStepEngine> stepEngine,
        std::shared_ptr<IGpioInput> endStopX,
        std::shared_ptr<IGpioInput> endStopY,
        std::shared_ptr<IGpioInput> endStopZ,
        std::shared_ptr<IComms> comms);
