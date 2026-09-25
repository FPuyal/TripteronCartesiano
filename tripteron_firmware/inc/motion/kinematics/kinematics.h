#pragma once

#include "kinematics_interface.h"
#include "kinematics_types.h"
#include "motion_controller_types.h"
#include <memory>
#include <array>

class Kinematics : public IKinematics {
public:
    Kinematics(std::shared_ptr<IEncoder> xEncoder, std::shared_ptr<IEncoder> yEncoder, std::shared_ptr<IEncoder> zEncoder) :
        mXEncoder(xEncoder), mYEncoder(yEncoder), mZEncoder(zEncoder) {}
    bool CaptureHome() override;
    bool Update() override;
    void RequestUpdate() override { mUpdateKinematics = true; }
    KinematicState GetCurrentState() const override { return mCurrentState; }

private:
    KinematicsResult CalculateKinematics();

    KinematicState mCurrentState {};
    MotionData mHome = {};

    static constexpr uint32_t kVelTimeLapse = 50;
    std::array<MotionData, kVelTimeLapse> mPreviousPositions{};

    std::shared_ptr<IEncoder> mXEncoder;
    std::shared_ptr<IEncoder> mYEncoder;
    std::shared_ptr<IEncoder> mZEncoder;

    volatile bool mUpdateKinematics = false;
};

// Observador Luenberger para estimar posición, velocidad y aceleración de cada eje del robot.
