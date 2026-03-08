#pragma once

#include "utils.h"

#include <memory>

class ITrajectoryProfile {
public:
    virtual ~ITrajectoryProfile() = default;
    virtual void SetTrajectorySegment(TrajectoryState init, TrajectoryState final) = 0;
    virtual bool Update(double dt) = 0;
    virtual double GetPosition() const = 0;
    virtual double GetVelocity() const = 0;
    virtual bool IsFinished() const = 0;
};

std::shared_ptr<ITrajectoryProfile> MakeITrajectoryProfile(TrajectoryConfig config);
