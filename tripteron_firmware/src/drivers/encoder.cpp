#include "encoder.h"
#include <cstdint>
#include <memory>

#define AS5600_RAW_ANGLE 0x0C

bool Encoder::SetOffset(){
    uint16_t init_raw_angle;

    mI2cWrapper->SetMemAddress(AS5600_RAW_ANGLE);
    if(!mI2cWrapper->Read(init_raw_angle))
        return false;
    mHomeRaw = init_raw_angle;
    return true;
}

bool Encoder::ReadAngle(float& angle){
    uint16_t raw_angle = 0;

    mI2cWrapper->SetMemAddress(AS5600_RAW_ANGLE);
    if(mI2cWrapper->Read(raw_angle)) {
        int32_t delta = (int32_t)raw_angle - (int32_t)mHomeRaw;
        delta = (delta + 4096) % 4096;
        angle = delta * 360.0f / 4096.0f;
        if(angle > 180.0f) angle -= 360.0f;
        return true;
    }
    return false;
}

std::shared_ptr<IEncoder> MakeIEncoder(std::shared_ptr<II2CWrapper> i2cWrapper) {
    return std::make_shared<Encoder>(i2cWrapper);
}
