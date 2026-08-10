#pragma once

#include "motion_controller_interface.h"
#include "motion_controller_utils.h"
#include "step_engine_interface.h"
#include "trajectory_generator_interface.h"

#include <array>
#include <cstddef>
#include <memory>

class MotionController : public IMotionController {
public:
    MotionController(std::shared_ptr<IStepEngine> stepEngine, MotionConfig motionConfig);
    void SetSegments(MotionData* segments, std::size_t numSegments) override;
    bool Move() override;
    void RequestUpdate() override { mUpdateMotion = true; }
    bool Update() override;

    MotionData GetPosition() override;
    MotionData GetVelocity() override;

private:
    SegmentData* mSegments = nullptr;
    std::size_t mNumSegments = 0;
    std::size_t mCurrentSegment = 0;

    // Estado del robot (mm, mm/s)
    MotionData mPosition = {};
    MotionData mVelocity = {};

    // Límites mecánicos y conversión por eje (fijos, mecánica del robot)
    const MotionConfig mMotionConfig = {};

    volatile bool mUpdateMotion = false; // bandera de actualización de movimiento

    std::shared_ptr<IStepEngine> mStepEngine;
    std::unique_ptr<ITrajectoryGenerator> mTrajectoryGenerator; // perfil maestro del segmento (distancia total)
};
