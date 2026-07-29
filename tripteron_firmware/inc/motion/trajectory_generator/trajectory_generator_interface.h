#pragma once

#include "trajectory_generator_utils.h"

#include <memory>

class ITrajectoryGenerator {
public:
    virtual ~ITrajectoryGenerator() = default;
    virtual bool SetTrajectoryProfile(MotionState init, MotionState final) = 0;
    virtual void RequestUpdate() = 0;
    virtual bool Update() = 0;
    virtual void Reset() = 0;

    virtual float GetDirection() const = 0;
    virtual float GetPosition() const = 0;
    virtual float GetVelocity() const = 0;

    virtual bool IsFinished() const = 0;
};

std::unique_ptr<ITrajectoryGenerator> MakeITrajectoryGenerator(TrajectoryConfig config);
