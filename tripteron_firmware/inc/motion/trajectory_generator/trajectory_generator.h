#pragma once

#include "trajectory_generator_interface.h"

class TrajectoryGenerator : public ITrajectoryGenerator {
public:
    explicit TrajectoryGenerator(TrajectoryConfig config) : mConfig(config) {}
    bool SetTrajectoryProfile(MotionState init, MotionState final);
    bool Update(double dt) override;
    void Reset() override;
    double GetPosition() const override { return mPos; }
    double GetVelocity() const override { return mVel; }
    bool IsFinished() const override { return mFinished; }

private:
    bool GeneratePhases();

    TrajectoryConfig mConfig;
    MotionState mInit;
    MotionState mFinal;

    TrayectoryProfileType mProfile;
    std::vector<TrayectoryPhase> mPhases;
    std::size_t mCurrentPhase = 0;

    // Initial phase variables
    double mPhaseTime = 0.0;
    double mPos0 = 0.0;
    double mVel0 = 0.0;

    // Actual state variables
    double mDir;
    double mPos;
    double mVel;
    double mAcc;

    bool mFinished;
};
