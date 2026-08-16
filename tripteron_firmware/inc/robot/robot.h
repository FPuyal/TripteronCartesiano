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

#include <cstdint>
#include <memory>

class Robot : public IRobot {
public:
    Robot(std::shared_ptr<IStepEngine> stepEngine,
        std::shared_ptr<IGpioInput> endStopX,
        std::shared_ptr<IGpioInput> endStopY,
        std::shared_ptr<IGpioInput> endStopZ,
        std::shared_ptr<IComms> comms);

    void Run() override {};
    void Tick() override;
    void SetCommandRequest(CommandRequest commandRequest) override;
    void RequestTelemetry() override;
    void EmergencyStop() override;
    State GetState() const override { return mState; }

private:
    bool SendTelemetryData();
    CommandRequest ParseCommand(uint8_t* command, uint16_t size);

    static constexpr uint16_t mMaxSegments = 20;

    volatile State mState = State::Init;
    StateRequest mStateRequest;
    MotionData mPath[mMaxSegments] = {};
    uint16_t mPathSize = 0;

    static constexpr uint16_t homingTimeoutMs = 10000;
    static constexpr uint16_t backoffTimeoutMs = 5000;

    uint32_t mElapsedMs = 0;
    uint32_t mHomingMs = 0;
    uint32_t mBackoffMs = 0;

    uint8_t mTelemetryBuffer[52] {};
    uint8_t mEndByte = 0xFF;
    bool mTelemetryFlag = false;

    std::shared_ptr<IMotionController> mMotionController;
    std::shared_ptr<IStepEngine> mStepEngine;
    std::shared_ptr<IGpioInput> mEndStopX;
    std::shared_ptr<IGpioInput> mEndStopY;
    std::shared_ptr<IGpioInput> mEndStopZ;
    std::shared_ptr<IComms> mComms;
};

