#include "motion_controller.h"
#include "motion_controller_types.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

MotionController::MotionController(MotionConfig motionConfig)
        : mMotionConfig(motionConfig) {
    mTrajectoryGenerator = MakeITrajectoryGenerator();
};

bool MotionController::SetSegments(MotionPath path) {
    if(path.size > kMaxSegments) {
        mNumSegments = 0;
        mFinished = true;
        return false;
    }

    mNumSegments = path.size;

    mFinished = false;

    for(uint8_t i = 0; i < mNumSegments; i++){
        if(path.segments[i].x > mMotionConfig.posMax.x || path.segments[i].y > mMotionConfig.posMax.y || path.segments[i].z > mMotionConfig.posMax.z ||
            path.segments[i].x < 0.0f || path.segments[i].y < 0.0f || path.segments[i].z < 0.0f) {
                mNumSegments = 0;
                mFinished = true;
                return false;
        }

        mSegments[i].posTarget = path.segments[i];
    }

    mCurrentSegment = 0;

    if(mNumSegments == 0)
        return false;

    // Geometría + límites de velocidad/aceleración propios de cada segmento (una sola vez).
    mPathInit = mPosition;
    MotionData ref = mPosition;
    for(uint8_t i = 0; i < mNumSegments; i++) {
        SegmentData& segment = mSegments[i];

        MotionData d = {segment.posTarget.x - ref.x, segment.posTarget.y - ref.y, segment.posTarget.z - ref.z};
        segment.dist = std::sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
        segment.cos  = (segment.dist > 1e-6f) ? MotionData{d.x/segment.dist, d.y/segment.dist, d.z/segment.dist}
                                      : MotionData{0.0f, 0.0f, 0.0f};

        segment.velMaxSeg = std::min({
            mMotionConfig.velMax.x / std::fabs(segment.cos.x),
            mMotionConfig.velMax.y / std::fabs(segment.cos.y),
            mMotionConfig.velMax.z / std::fabs(segment.cos.z)
        });
        segment.accMaxSeg = std::min({
            mMotionConfig.accMax.x / std::fabs(segment.cos.x),
            mMotionConfig.accMax.y / std::fabs(segment.cos.y),
            mMotionConfig.accMax.z / std::fabs(segment.cos.z)
        });

        ref = segment.posTarget;
    }

    // Look-ahead, pasada hacia atrás: velocidad de cruce (junction) capada por lo que
    // el segmento siguiente necesita para poder frenar a tiempo hasta la suya.
    mSegments[mNumSegments-1].finalVel = 0.0f; // el último segmento de la cola siempre frena del todo

    for(int i = mNumSegments - 2; i >= 0; i--) {
        SegmentData& segment = mSegments[i];
        const SegmentData& nextSegment = mSegments[i+1];

        float cosTheta = -(segment.cos.x * nextSegment.cos.x + segment.cos.y * nextSegment.cos.y + segment.cos.z * nextSegment.cos.z);
        cosTheta = std::min(1.0f, std::max(-1.0f, cosTheta));
        const float sinMidTheta = std::sqrt(0.5f * (1.0f - cosTheta));

        float junctionVel = segment.velMaxSeg; // recto: no hay restricción de esquina
        if(sinMidTheta < 1.0f - 1e-6f) {
            const float R = mMotionConfig.junctionDeviation * sinMidTheta / (1.0f - sinMidTheta);
            junctionVel = std::sqrt(segment.accMaxSeg * R);
        }

        const float velDecelLimit = std::sqrt(nextSegment.finalVel * nextSegment.finalVel + 2.0f * nextSegment.accMaxSeg * nextSegment.dist);

        segment.finalVel = std::min({junctionVel, segment.velMaxSeg, velDecelLimit});
    }

    // Look-ahead, pasada hacia adelante: capa por lo que de verdad se puede acelerar
    // desde la velocidad de entrada real en la distancia de cada segmento.
    float initVel = std::sqrt(mVelocity.x*mVelocity.x + mVelocity.y*mVelocity.y + mVelocity.z*mVelocity.z);
    for(uint8_t i = 0; i < mNumSegments; i++) {
        SegmentData& segment = mSegments[i];
        const float velAccelLimit = std::sqrt(initVel*initVel + 2.0f * segment.accMaxSeg * segment.dist);
        segment.finalVel = std::min(segment.finalVel, velAccelLimit);
        initVel = segment.finalVel;
    }

    return true;
}

bool MotionController::Move() {
    if(!mTrajectoryGenerator->IsFinished())
        return true; // segmento en curso, nada que lanzar todavía

    if(mCurrentSegment >= mNumSegments) {
        mFinished = true;
        return false;
    }

    const SegmentData& segment = mSegments[mCurrentSegment];

    if(segment.posTarget.x < 0.0f || segment.posTarget.x > mMotionConfig.posMax.x ||
        segment.posTarget.y < 0.0f || segment.posTarget.y > mMotionConfig.posMax.y ||
        segment.posTarget.z < 0.0f || segment.posTarget.z > mMotionConfig.posMax.z)
        return false;

    const float initVel = std::sqrt(mVelocity.x*mVelocity.x + mVelocity.y*mVelocity.y + mVelocity.z*mVelocity.z);

    mCurrentSegment++;

    return mTrajectoryGenerator->SetTrajectoryProfile(
        MotionState{0.0f, initVel},
        MotionState{segment.dist, segment.finalVel},
        TrajectoryConfig{segment.velMaxSeg, segment.accMaxSeg});
}

bool MotionController::Update() {
    if(!mUpdateMotion)
        return false;

    mUpdateMotion = false;

    if (!mTrajectoryGenerator->Update())
        return false;

    const float pathVel = mTrajectoryGenerator->GetVelocity();
    const float pathPos = mTrajectoryGenerator->GetPosition();

    uint8_t currentSegment = mCurrentSegment == 0 ? 0 : (mCurrentSegment - 1); // segmento actualmente en ejecución (mCurrentSegment ya apunta al siguiente)

    MotionData initPos = (currentSegment == 0) ? mPathInit : mSegments[currentSegment-1].posTarget;

    mPosition = {initPos.x + pathPos * mSegments[currentSegment].cos.x,
        initPos.y + pathPos * mSegments[currentSegment].cos.y,
        initPos.z + pathPos * mSegments[currentSegment].cos.z};

    mVelocity = {pathVel * mSegments[currentSegment].cos.x,
        pathVel * mSegments[currentSegment].cos.y,
        pathVel * mSegments[currentSegment].cos.z};

    return true;
}

std::shared_ptr<IMotionController> MakeIMotionController(MotionConfig motionConfig) {
    return std::make_shared<MotionController>(motionConfig);
}


