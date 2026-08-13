#pragma once

#include "comms.h"
#include "gpio_input_interface.h"
#include "kinematics_interface.h"
#include "motion_controller.h"
#include "motion_controller_interface.h"
#include "robot_interface.h"
#include "step_engine_interface.h"
#include "comms_interface.h"

#include "robot_types.h"

#include <memory>

class Robot : public IRobot {
public:
    Robot(std::shared_ptr<IStepEngine> stepEngine,
        std::shared_ptr<IGpioInput> endStopX,
        std::shared_ptr<IGpioInput> endStopY,
        std::shared_ptr<IGpioInput> endStopZ,
        std::shared_ptr<IComms> comms);

    void Run() override;
    void Tick() override;
    void SetCommandRequest(CommandRequest commandRequest) override;
    void EmergencyStop() override;

private:
    CommandRequest ParseCommand(char* command, uint16_t size);

    volatile State mState = StateRequest::Init;
    StateRequest mStateRequest;
    MotionData mPath[MAX_SEGMENTS] = {};
    uint16_t mPathSize = 0;

    uint32_t mHomingTicks = 0;
    uint32_t mBackoffTicks = 0;

    std::shared_ptr<IMotionController> mMotionController;
    std::shared_ptr<IStepEngine> mStepEngine;
    std::shared_ptr<IGpioInput> mEndStopX;
    std::shared_ptr<IGpioInput> mEndStopY;
    std::shared_ptr<IGpioInput> mEndStopZ;
    std::shared_ptr<IComms> mComms;
};

