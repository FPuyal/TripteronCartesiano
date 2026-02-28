#include "encoder.h"
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

bool Encoder::ReadAngle(uint16_t& angle){
    mI2cWrapper->SetMemAddress(AS5600_ANGLE);
    return mI2cWrapper->Read(angle);
}

std::shared_ptr<IEncoder> MakeIEncoder(std::shared_ptr<II2CWrapper> i2cWrapper) {
    return std::make_shared<Encoder>(i2cWrapper);
}
