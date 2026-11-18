#pragma once

#include "position_controller_types.h"

#include <memory>

class IPositionController {
public:
    virtual ~IPositionController() = default;
    virtual MotionData Update(const MotionData& posRef, const MotionData& velRef, const MotionData& posMeas) = 0;
    virtual void Reset() = 0;

    virtual MotionData GetError() const = 0;
    virtual bool IsSettled() const = 0;
};

std::shared_ptr<IPositionController> MakeIPositionController();
