#include "position_controller.h"

#include <algorithm>
#include <cmath>

// vCmd = vRef (feedforward) + Kp·(pRef - pMeas)
MotionData PositionController::Update(const MotionData& posRef, const MotionData& velRef, const MotionData& posMeas) {
    mError = {posRef.x - posMeas.x, posRef.y - posMeas.y, posRef.z - posMeas.z};

    // Corrección de un eje: P con zona muerta y saturación
    auto pCorrect = [](float error, float kp, float deadband, float maxCorrection) {
        if(std::fabs(error) < deadband)
            return 0.0f;
        return std::clamp(kp * error, -maxCorrection, maxCorrection);
    };

    return {
        velRef.x + pCorrect(mError.x, kConfig.kp.x, kConfig.deadband.x, kConfig.maxCorrection.x),
        velRef.y + pCorrect(mError.y, kConfig.kp.y, kConfig.deadband.y, kConfig.maxCorrection.y),
        velRef.z + pCorrect(mError.z, kConfig.kp.z, kConfig.deadband.z, kConfig.maxCorrection.z)};
}

bool PositionController::IsSettled() const {
    return std::fabs(mError.x) < kConfig.deadband.x &&
           std::fabs(mError.y) < kConfig.deadband.y &&
           std::fabs(mError.z) < kConfig.deadband.z;
}

std::shared_ptr<IPositionController> MakeIPositionController() {
    return std::make_shared<PositionController>();
}
