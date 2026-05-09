#pragma once

#include "trajectory_generator_utils.h"

#include <memory>

class ITrajectoryGenerator {
public:
    virtual ~ITrajectoryGenerator() = default;
    virtual bool SetTrajectoryProfile(MotionState init, MotionState final) = 0;
    virtual bool Update(double dt) = 0;
    virtual double GetPosition() const = 0;
    virtual double GetVelocity() const = 0;
    virtual bool IsFinished() const = 0;
};

std::shared_ptr<ITrajectoryGenerator> MakeITrajectoryGenerator(TrajectoryConfig config);
