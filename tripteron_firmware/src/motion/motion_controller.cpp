#include "motion_controller.h"

#include <algorithm>
#include <cmath>
#include <limits>

MotionController::MotionController(std::shared_ptr<IStepEngine> stepEngine, MotionConfig motionConfig)
        : mStepEngine(stepEngine),
          mPosMax{motionConfig.pos.x, motionConfig.pos.y, motionConfig.pos.z},
          mVelMax{motionConfig.vel.x, motionConfig.vel.y, motionConfig.vel.z},
          mAccMax{motionConfig.acc.x, motionConfig.acc.y, motionConfig.acc.z},
          mStepsPerMm{motionConfig.stepsPerMm.x, motionConfig.stepsPerMm.y, motionConfig.stepsPerMm.z} {
    mTrajectoryGenerator = MakeITrajectoryGenerator();
};

bool MotionController::MoveTo(MotionData posTarget) {
    if(posTarget.x < 0.0f || posTarget.y < 0.0f || posTarget.z < 0.0f || posTarget.x > mPosMax.x || posTarget.y > mPosMax.y || posTarget.z > mPosMax.z)
        return false;

    mCurrentTarget = posTarget;

    MotionData deltaPos = {mCurrentTarget.x - mPosition.x, mCurrentTarget.y - mPosition.y, mCurrentTarget.z - mPosition.z};
    float dist = 0.0f;
    dist += deltaPos.x * deltaPos.x;
    dist += deltaPos.y * deltaPos.y;
    dist += deltaPos.z * deltaPos.z;
    dist = std::sqrt(dist);

    if (dist < 1e-6f)
        return false; // Sin movimiento en este segmento.

    mCos.x = deltaPos.x / dist;
    mCos.y = deltaPos.y / dist;
    mCos.z = deltaPos.z / dist;

    float velMaxSeg = std::min({mVelMax.x / std::fabs(mCos.x), mVelMax.y / std::fabs(mCos.y), mVelMax.z / std::fabs(mCos.z)});
    float accMaxSeg = std::min({mAccMax.x / std::fabs(mCos.x), mAccMax.y / std::fabs(mCos.y), mAccMax.z / std::fabs(mCos.z)});

    // Velocidad de entrada al segmento: proyección de la velocidad actual sobre la nueva dirección.
    float currentSpeed = 0.0f;
    currentSpeed += mVelocity.x * mCos.x;
    currentSpeed += mVelocity.y * mCos.y;
    currentSpeed += mVelocity.z * mCos.z;

    if (currentSpeed < 0.0f)
        currentSpeed = 0.0f;

    mSegmentStart = mPosition;

    return mTrajectoryGenerator->SetTrajectoryProfile(
        MotionState{0.0f, currentSpeed},
        MotionState{dist, 0.0f},
        TrajectoryConfig{velMaxSeg, accMaxSeg}
    );
}

bool MotionController::Update() {
    if(!mUpdateMotion)
        return false;

    mUpdateMotion = false;

    if (!mTrajectoryGenerator->Update())
        return false;

    const float pathVel = mTrajectoryGenerator->GetVelocity();
    const float pathPos = mTrajectoryGenerator->GetPosition();

    mPosition = {mSegmentStart.x + pathPos * mCos.x, mSegmentStart.y + pathPos * mCos.y, mSegmentStart.z + pathPos * mCos.z};
    mVelocity = {pathVel * mCos.x, pathVel * mCos.y, pathVel * mCos.z};

    mStepEngine->SetXSteps(static_cast<int16_t>(mVelocity.x * mStepsPerMm.x));
    mStepEngine->SetYSteps(static_cast<int16_t>(mVelocity.y * mStepsPerMm.y));
    mStepEngine->SetZSteps(static_cast<int16_t>(mVelocity.z * mStepsPerMm.z));

    return true;
}

MotionData MotionController::GetPosition() {
    return mPosition;
}

MotionData MotionController::GetVelocity() {
    return mVelocity;
}

std::shared_ptr<IMotionController> MakeIMotionController(std::shared_ptr<IStepEngine> stepEngine, MotionConfig motionConfig) {
    return std::make_shared<MotionController>(stepEngine, motionConfig);
}



