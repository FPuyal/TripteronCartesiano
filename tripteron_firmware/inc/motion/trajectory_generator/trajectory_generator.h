#pragma once

#include "trajectory_generator_interface.h"

class TrajectoryGenerator : public ITrajectoryGenerator {
public:
    TrajectoryGenerator() = default;
    bool SetTrajectoryProfile(MotionState init, MotionState final, TrajectoryConfig config) override;
    bool Update() override;
    bool IsFinished() override { return mFinished; }    
    void Reset() override;

    float GetPosition() const override { return mPos; }
    float GetVelocity() const override { return mVel; }

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
    volatile float mPos = 0.0;
    volatile float mVel = 0.0;
    float mAcc = 0.0;

    volatile bool mFinished = true;
};
