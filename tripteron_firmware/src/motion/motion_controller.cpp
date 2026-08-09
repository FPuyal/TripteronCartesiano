#include "motion_controller.h"

#include <algorithm>
#include <cmath>
#include <limits>

MotionController::MotionController(std::shared_ptr<IStepEngine> stepEngine, MotionConfig motionConfig)
        : mStepEngine(stepEngine),
          mPosMax{motionConfig.posMax.x, motionConfig.posMax.y, motionConfig.posMax.z},
          mVelMax{motionConfig.velMax.x, motionConfig.velMax.y, motionConfig.velMax.z},
          mVelMin{motionConfig.velMin.x, motionConfig.velMin.y, motionConfig.velMin.z},
          mAccMax{motionConfig.accMax.x, motionConfig.accMax.y, motionConfig.accMax.z},
          mStepsPerMm{motionConfig.stepsPerMm.x, motionConfig.stepsPerMm.y, motionConfig.stepsPerMm.z} {
    mTrajectoryGenerator = MakeITrajectoryGenerator();
};

void MotionController::SetSegments(MotionData* segments, std::size_t numSegments) {
    mSegments = segments;
    mNumSegments = numSegments;
    mCurrentSegment = 0;
}

bool MotionController::Move() {
    if(!mTrajectoryGenerator->IsFinished())
        return true; // Segmento en curso, todavía hay movimiento pendiente.

    if(!mSegments || mCurrentSegment >= mNumSegments)
        return false; // No queda nada por lanzar y el último segmento ya terminó.

    if(!MoveTo(mSegments[mCurrentSegment++])) {
        mSegments = nullptr; // Aborta la cola: evita quedarse bloqueado en silencio.
        mNumSegments = 0;
        mCurrentSegment = 0;
        return false;
    }

    return true;
}

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

    auto axisMinSpeed = [](float minSpeed, float cos) {
        float minCosThreshold = 0.001f; // ajustable
        float cosAbs = std::fabs(cos) < minCosThreshold ? minCosThreshold : std::fabs(cos);
        return minSpeed / cosAbs;
    };

    float maxFinalSpeedSeg = std::min({
        axisMinSpeed(mVelMin.x, mCos.x),
        axisMinSpeed(mVelMin.y, mCos.y),
        axisMinSpeed(mVelMin.z, mCos.z)
    });

    float currentSpeedSeg = maxFinalSpeedSeg; // velocidad inicial del segmento: la velocidad final del segmento anterior

    mSegmentStart = mPosition;

    if(mCurrentSegment >= mNumSegments)
        maxFinalSpeedSeg = 0.0f; // último segmento: desacelerar hasta detenerse

    return mTrajectoryGenerator->SetTrajectoryProfile(
        MotionState{0.0f, currentSpeedSeg},
        MotionState{dist, maxFinalSpeedSeg},
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


