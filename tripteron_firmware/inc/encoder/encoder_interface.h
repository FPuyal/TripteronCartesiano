#pragma once

#include "i2c_wrapper_interface.h"

#include <memory>

class IEncoder {
public:
    virtual ~IEncoder() = default;
    virtual bool SetOffset() = 0;
    virtual bool ReadAngle(double& angle) = 0;
};

std::shared_ptr<IEncoder> MakeIEncoder(std::shared_ptr<II2CWrapper> i2cWrapper);
