#pragma once

#include "encoder_interface.h"

class Encoder : public IEncoder {
public:
    Encoder(std::shared_ptr<II2CWrapper> i2cWrapper) :
        mI2cWrapper(i2cWrapper) {}
    bool SetOffset() override;
    bool ReadAngle(float& angle) override;
    
private:
    std::shared_ptr<II2CWrapper> mI2cWrapper;
};
