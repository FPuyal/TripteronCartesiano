#pragma once

#include "motion_controller_interface.h"
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
    bool MoveTo(const MotionData posTarget) override;
    void RequestUpdate() override { mUpdateMotion = true; }
    bool Update() override;

    MotionData GetPosition() override;
    MotionData GetVelocity() override;

private:
    MotionData* mSegments = nullptr;
    std::size_t mNumSegments = 0;
    std::size_t mCurrentSegment = 0;

    // Estado del robot (mm, mm/s)
    MotionData mPosition = {0.0f, 0.0f, 0.0f};
    MotionData mVelocity = {0.0f, 0.0f, 0.0f};

    // Segmento en curso
    MotionData mCurrentTarget = {0.0f, 0.0f, 0.0f};
    MotionData mSegmentStart = {0.0f, 0.0f, 0.0f};
    MotionData mCos = {0.0f, 0.0f, 0.0f}; // ponderación geométrica (coseno director por eje)

    // Límites mecánicos y conversión por eje (fijos, mecánica del robot)
    const MotionData mPosMax;
    const MotionData mVelMax;
    const MotionData mVelMin;
    const MotionData mAccMax;
    const MotionData mStepsPerMm; // ponderación mecánica: mm/s -> tasa de StepEngine

    volatile bool mUpdateMotion = false; // bandera de actualización de movimiento

    std::shared_ptr<IStepEngine> mStepEngine;
    std::unique_ptr<ITrajectoryGenerator> mTrajectoryGenerator; // perfil maestro del segmento (distancia total)
};
