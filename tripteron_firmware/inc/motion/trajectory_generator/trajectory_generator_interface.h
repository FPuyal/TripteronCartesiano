#pragma once

#include "trajectory_generator_types.h"

#include <memory>

class ITrajectoryGenerator {
public:
    virtual ~ITrajectoryGenerator() = default;
    virtual bool SetTrajectoryProfile(MotionState init, MotionState final, TrajectoryConfig config) = 0;
    virtual bool Update() = 0;
    virtual bool IsFinished() = 0;
    virtual void Reset() = 0;

    virtual float GetPosition() const = 0;
    virtual float GetVelocity() const = 0;
};

std::unique_ptr<ITrajectoryGenerator> MakeITrajectoryGenerator();
