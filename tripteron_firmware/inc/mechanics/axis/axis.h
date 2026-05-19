#pragma once

#include "axis_interface.h"
#include "axis_types.h"
#include "trajectory_generator_utils.h"
#include <memory>
#include <sys/types.h>

class Axis : public IAxis {
public:
    Axis(std::shared_ptr<IGpioInput> gpioInput, std::shared_ptr<ITmc> tmc, const AxisConfig& config);
    void Tick() override;
    bool RequestState(AxisCommandRequest command) override;
    bool SetSegment(const std::vector<MotionState>& segment) override;
    void EmergencyStop() override;

    AxisState GetState() const override;
    double GetPosition() const override;
    double GetVelocity() const override;

private:
    std::unique_ptr<ITrajectoryGenerator> mTrajectoryGenerator;
    std::shared_ptr<IGpioInput> mEndStop;
    std::shared_ptr<ITmc> mTmc;

    AxisState mState = AxisState::Disabled;
    AxisCommandRequest mCommandRequest = AxisCommandRequest::None;
    MotionState mCurrentMotionState = {0.0, 0.0};
    std::vector<MotionState> mSegment = {};
    size_t mCurrentSegment = 0;

    double mStepsMmRatio;
    double mMaxPosition;
    uint32_t mHomingTicks = 0;
};
