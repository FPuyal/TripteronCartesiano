#pragma once

#include "motion_controller_interface.h"
#include "motion_controller_types.h"
#include "trajectory_generator_interface.h"

#include <array>
#include <cstdint>
#include <memory>

class MotionController : public IMotionController {
public:
    MotionController(MotionConfig motionConfig);
    void SetHomePosition() override { mPosition = {0.0f, 0.0f, 0.0f}; mVelocity = {0.0f, 0.0f, 0.0f};}
    bool SetSegments(MotionPath path) override;
    bool Move() override;
    void RequestUpdate() override { mUpdateMotion = true; }
    bool Update() override;

    MotionData GetPosition() override { return mPosition; }
    MotionData GetVelocity() override { return mVelocity; }
    MotionData GetSteps() override { return mVelocity * mMotionConfig.stepsPerMm; }
    bool IsFinished() override { return mFinished; }

private:
    static_assert(kMaxSegments <= 255, "mNumSegments/mCurrentSegment son uint8_t");
    std::array<SegmentData, kMaxSegments> mSegments {};
    uint8_t mNumSegments = 0;
    uint8_t mCurrentSegment = 0;
    bool mFinished = true;

    // Estado del robot (mm, mm/s)
    MotionData mPathInit = {};
    MotionData mPosition = {};
    MotionData mVelocity = {};

    // Límites mecánicos y conversión por eje (fijos, mecánica del robot)
    const MotionConfig mMotionConfig = {};

    volatile bool mUpdateMotion = false; // bandera de actualización de movimiento

    std::unique_ptr<ITrajectoryGenerator> mTrajectoryGenerator; // perfil maestro del segmento (distancia total)
};
