#pragma once

#include "trayectory_profile_interface.h"

class TrajectoryProfile : public ITrajectoryProfile {
public:
    TrajectoryProfile(TrajectoryConfig config) : mVelMax(config.velMax), mAccMax(config.accMax), mJerk(config.jerk) {}
    void SetTrajectorySegment(TrajectoryState init, TrajectoryState final) override;
    bool Update(double dt) override;
    double GetPosition() const override { return mPos; }
    double GetVelocity() const override { return mVel; }
    bool IsFinished() const override { return mFinished; }

private:

    double mDir;

    double mVelMax;
    double mAccMax;
    double mJerk;

    double mPos;
    double mVel;
    double mAcc;

    double t1, t2, t3;

    double mTotalTime;

    double mTime = 0.0;

    bool mFinished = true;
};
