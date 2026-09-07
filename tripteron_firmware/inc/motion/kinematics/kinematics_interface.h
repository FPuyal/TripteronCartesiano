#pragma once

#include "encoder_interface.h"
#include "kinematics_types.h"

#include <memory>

class IKinematics {
public:
    virtual ~IKinematics() = default;
    virtual bool CaptureHome() = 0;
    virtual bool Update() = 0;
    virtual void RequestUpdate() = 0;
    virtual KinematicState GetCurrentState() const = 0;
};

std::shared_ptr<IKinematics> MakeIKinematics(std::shared_ptr<IEncoder> xEncoder, std::shared_ptr<IEncoder> yEncoder, std::shared_ptr<IEncoder> zEncoder);
