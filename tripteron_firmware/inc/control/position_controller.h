#pragma once

#include "position_controller_interface.h"
#include "position_controller_types.h"

class PositionController : public IPositionController {
public:
    PositionController() = default;
    MotionData Update(const MotionData& posRef, const MotionData& velRef, const MotionData& posMeas) override;
    void Reset() override { mError = {}; }

    MotionData GetError() const override { return mError; }
    bool IsSettled() const override;

private:
    static constexpr PositionControlConfig kConfig {
        {5.0f, 5.0f, 5.0f},    // kp (1/s)
        {10.0f, 10.0f, 5.0f},  // maxCorrection (mm/s)
        {0.5f, 0.5f, 0.5f}};   // deadband (mm), > resolución del encoder en cartesianas

    MotionData mError = {};
};
