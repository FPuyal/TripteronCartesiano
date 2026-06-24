#pragma once

#include "encoder_interface.h"

#include <memory>

struct CinematicState {
    int32_t pos[3];
    int32_t vel[3];
};

class IKinematics {
public:
    virtual ~IKinematics() = default;
    virtual CinematicState Update() = 0;
};

std::shared_ptr<IKinematics> MakeIKinematics(std::shared_ptr<IEncoder> xEncoder, std::shared_ptr<IEncoder> yEncoder, std::shared_ptr<IEncoder> zEncoder);
