#include "motion_controller.h"
#include "motion_controller_utils.h"

#include <algorithm>
#include <cmath>
#include <limits>

MotionController::MotionController(std::shared_ptr<IStepEngine> stepEngine, MotionConfig motionConfig)
        : mStepEngine(stepEngine), mMotionConfig(motionConfig) {
    mTrajectoryGenerator = MakeITrajectoryGenerator();
};

void MotionController::SetSegments(MotionData* segments, std::size_t numSegments) {
    delete[] mSegments;
    mSegments = new SegmentData[numSegments]{};
    mNumSegments = numSegments;

    for(int i = 0; i < mNumSegments; i++)
        mSegments[i].posTarget = segments[i];

    mCurrentSegment = 0;

    MotionData ref = mPosition;
    for(int i = 0; i < mNumSegments; i++) {
        MotionData d = {mSegments[i].posTarget.x - ref.x, mSegments[i].posTarget.y - ref.y, mSegments[i].posTarget.z - ref.z};
        float dist = std::sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
        mSegments[i].dist = dist;
        mSegments[i].cos  = (dist > 1e-6f) ? MotionData{d.x/dist, d.y/dist, d.z/dist}
                                : MotionData{0.0f, 0.0f, 0.0f};
        ref = mSegments[i].posTarget;
    }
}

bool MotionController::Move() {
    if(!mSegments || mCurrentSegment >= mNumSegments)
        return false;

    if(mTrajectoryGenerator->IsFinished()) {
        if(mSegments[mCurrentSegment].posTarget.x < 0.0f || mSegments[mCurrentSegment].posTarget.x > mMotionConfig.posMax.x ||
            mSegments[mCurrentSegment].posTarget.y < 0.0f || mSegments[mCurrentSegment].posTarget.y > mMotionConfig.posMax.y ||
            mSegments[mCurrentSegment].posTarget.z < 0.0f || mSegments[mCurrentSegment].posTarget.z > mMotionConfig.posMax.z)
            return false;

        float velMaxSeg = std::min({
            mMotionConfig.velMax.x / std::fabs(mSegments[mCurrentSegment].cos.x),
            mMotionConfig.velMax.y / std::fabs(mSegments[mCurrentSegment].cos.y),
            mMotionConfig.velMax.z / std::fabs(mSegments[mCurrentSegment].cos.z)
        });

        float accMaxSeg = std::min({
            mMotionConfig.accMax.x / std::fabs(mSegments[mCurrentSegment].cos.x),
            mMotionConfig.accMax.y / std::fabs(mSegments[mCurrentSegment].cos.y),
            mMotionConfig.accMax.z / std::fabs(mSegments[mCurrentSegment].cos.z)
        });

        float initVel = sqrt(mVelocity.x * mVelocity.x + mVelocity.y * mVelocity.y + mVelocity.z * mVelocity.z);
        float finalVel = 0.0f;

        if(mCurrentSegment < (mNumSegments-1)) {
            float cosTheta = -(mSegments[mCurrentSegment].cos.x * mSegments[mCurrentSegment+1].cos.x +
                    mSegments[mCurrentSegment].cos.y * mSegments[mCurrentSegment+1].cos.y +
                    mSegments[mCurrentSegment].cos.z * mSegments[mCurrentSegment+1].cos.z);

            if(cosTheta > 1.0f) cosTheta = 1.0f;
            if(cosTheta < -1.0f) cosTheta = -1.0f;

            float sinMidTheta = std::sqrt(0.5f * (1.0f - cosTheta));
            if(sinMidTheta < 1.0f - 1e-6f) {
                float R = mMotionConfig.junctionDeviation * sinMidTheta / (1.0f - sinMidTheta);
                finalVel = std::sqrt(accMaxSeg * R);
            } else {
                finalVel = velMaxSeg;   // recto
            }
            finalVel = std::min(finalVel, velMaxSeg);   // (idealmente también con la velMax del siguiente segmento)
        }

        mCurrentSegment++;

        return mTrajectoryGenerator->SetTrajectoryProfile(
            MotionState{0.0f, initVel},
            MotionState{mSegments[mCurrentSegment-1].dist, finalVel},
            TrajectoryConfig{velMaxSeg, accMaxSeg});
    }

    return true;
}

bool MotionController::Update() {
    if(!mUpdateMotion)
        return false;

    mUpdateMotion = false;

    if (!mTrajectoryGenerator->Update())
        return false;

    const float pathVel = mTrajectoryGenerator->GetVelocity();
    const float pathPos = mTrajectoryGenerator->GetPosition();

    const std::size_t idx = mCurrentSegment - 1; // segmento actualmente en ejecución (mCurrentSegment ya apunta al siguiente)

    MotionData initPos = {0.0f, 0.0f, 0.0f};
    if(idx != 0)
        initPos = mSegments[idx-1].posTarget;

    mPosition = {initPos.x + pathPos * mSegments[idx].cos.x,
        initPos.y + pathPos * mSegments[idx].cos.y,
        initPos.z + pathPos * mSegments[idx].cos.z};

    mVelocity = {pathVel * mSegments[idx].cos.x,
        pathVel * mSegments[idx].cos.y,
        pathVel * mSegments[idx].cos.z};

    mStepEngine->SetXSteps(static_cast<int16_t>(mVelocity.x * mMotionConfig.stepsPerMm.x));
    mStepEngine->SetYSteps(static_cast<int16_t>(mVelocity.y * mMotionConfig.stepsPerMm.y));
    mStepEngine->SetZSteps(static_cast<int16_t>(mVelocity.z * mMotionConfig.stepsPerMm.z));

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


