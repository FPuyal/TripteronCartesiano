#pragma once

#include "encoder_interface.h"

#include <memory>

struct KinematicState {
    float pos[3];
    float vel[3];
};

class IKinematics {
public:
    virtual ~IKinematics() = default;
    virtual void CaptureHome() = 0;
    virtual void Update() = 0;
    virtual void RequestUpdate() = 0;
    virtual KinematicState GetCurrentState() const = 0;
};

std::shared_ptr<IKinematics> MakeIKinematics(std::shared_ptr<IEncoder> xEncoder, std::shared_ptr<IEncoder> yEncoder, std::shared_ptr<IEncoder> zEncoder);
