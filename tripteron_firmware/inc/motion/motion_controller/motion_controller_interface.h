#pragma once

#include "step_engine_interface.h"
#include "motion_controller_utils.h"

#include <array>
#include <memory>

class IMotionController {
public:
    virtual ~IMotionController() = default;
    virtual void SetHomePosition() = 0; // referencia real tras homing: pos y vel de partida para SetSegments()
    virtual void SetSegments(MotionData* segments, std::size_t numSegments) = 0;
    virtual bool Move() = 0;
    virtual void RequestUpdate() = 0;
    virtual bool Update() = 0;

    virtual MotionData GetPosition() = 0;
    virtual MotionData GetVelocity() = 0;
    virtual MotionData GetSteps() = 0;
};

std::shared_ptr<IMotionController> MakeIMotionController(MotionConfig motionConfig);
