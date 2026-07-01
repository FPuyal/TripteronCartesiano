#include "encoder.h"
#include <cstdint>
#include <memory>

#define AS5600_RAW_ANGLE 0x0C
#define AS5600_ANGLE 0x0E
#define ZPOS_H 0x01

bool Encoder::SetOffset(){
    uint16_t init_raw_angle;

    mI2cWrapper->SetMemAddress(AS5600_RAW_ANGLE);
    if(!mI2cWrapper->Read(init_raw_angle))
        return false;
    mI2cWrapper->SetMemAddress(ZPOS_H);
    if(mI2cWrapper->Write(init_raw_angle))
        return false;
    return true;
}

bool Encoder::ReadAngle(float& angle){
    uint16_t raw_angle = 0;

    mI2cWrapper->SetMemAddress(AS5600_ANGLE);
    if(mI2cWrapper->Read(raw_angle)) {
        angle = raw_angle * 360.0f / 4096.0f;
        return true;
    }
    return false;
}

std::shared_ptr<IEncoder> MakeIEncoder(std::shared_ptr<II2CWrapper> i2cWrapper) {
    return std::make_shared<Encoder>(i2cWrapper);
}
