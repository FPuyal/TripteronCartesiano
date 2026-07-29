#pragma once

#include "trajectory_generator_interface.h"

class TrajectoryGenerator : public ITrajectoryGenerator {
public:
    explicit TrajectoryGenerator(TrajectoryConfig config) : mConfig(config) {}
    bool SetTrajectoryProfile(MotionState init, MotionState final);
    void RequestUpdate() override { mUpdateTrajectory = true; }
    bool Update() override;
    void Reset() override;

    float GetDirection() const override { return mDir; }
    float GetPosition() const override { return mPos; }
    float GetVelocity() const override { return mVel; }

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
    float mPhaseTime = 0.0;
    float mPos0 = 0.0;
    float mVel0 = 0.0;

    // Actual state variables
    volatile float mDir = 0.0;
    volatile float mPos = 0.0;
    volatile float mVel = 0.0;
    float mAcc = 0.0;

    volatile bool mFinished = true;

    volatile bool mUpdateTrajectory = false;
};
