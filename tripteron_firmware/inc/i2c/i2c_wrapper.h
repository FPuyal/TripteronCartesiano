#pragma once

#include "i2c_wrapper_interface.h"

class I2CWrapper : public II2CWrapper {
public:
    I2CWrapper(I2C_HandleTypeDef* hi2c, uint16_t devAddress) :
        mHi2c(hi2c),
        mDevAddress(devAddress),
        mMemAddress(0) {}
    bool Read(uint16_t& raw_value) override;
    bool Write(uint16_t raw_value) override;
    void SetMemAddress(uint8_t addr) override;
private:
    I2C_HandleTypeDef* mHi2c;
    uint16_t mDevAddress;
    uint8_t mMemAddress;
};
