#pragma once

#include "kinematics_interface.h"
#include "kinematics_types.h"
#include "motion_controller_types.h"
#include <memory>

class Kinematics : public IKinematics {
public:
    Kinematics(std::shared_ptr<IEncoder> xEncoder, std::shared_ptr<IEncoder> yEncoder, std::shared_ptr<IEncoder> zEncoder) :
        mXEncoder(xEncoder), mYEncoder(yEncoder), mZEncoder(zEncoder) {}
    void CaptureHome() override;
    void Update() override;
    void RequestUpdate() override { mUpdateKinematics = true; }
    KinematicState GetCurrentState() const override { return mCurrentState; }

private:
    void CalculateKinematics();

    KinematicState mCurrentState {};
    KinematicState mPreviousState {};
    MotionData mHome = {};

    std::shared_ptr<IEncoder> mXEncoder;
    std::shared_ptr<IEncoder> mYEncoder;
    std::shared_ptr<IEncoder> mZEncoder;

    volatile bool mUpdateKinematics = false;
};

// Observador Luenberger para estimar posición, velocidad y aceleración de cada eje del robot.
