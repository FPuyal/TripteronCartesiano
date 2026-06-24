#pragma once

#include "kinematics_interface.h"
#include <memory>

class Kinematics : public IKinematics {
public:
    Kinematics(std::shared_ptr<IEncoder> xEncoder, std::shared_ptr<IEncoder> yEncoder, std::shared_ptr<IEncoder> zEncoder) :
        mXEncoder(xEncoder), mYEncoder(yEncoder), mZEncoder(zEncoder) {}
    CinematicState Update() override;

private:
    CinematicState mPreviousState{ {0, 0, 0}, {0, 0, 0} };

    std::shared_ptr<IEncoder> mXEncoder;
    std::shared_ptr<IEncoder> mYEncoder;
    std::shared_ptr<IEncoder> mZEncoder;
};

// Observador Luenberger para estimar posición, velocidad y aceleración de cada eje del robot.
