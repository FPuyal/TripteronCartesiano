#include "i2c_wrapper.h"

bool I2CWrapper::Read(uint16_t& raw_value) {
    if(!mMemAddress)
        return false;

    uint8_t buffer[2];

    // HAL_StatusTypeDef lectura = HAL_I2C_Mem_Read(mHi2c, mDevAddress, mMemAddress, I2C_MEMADD_SIZE_8BIT, buffer, 2, 10);

    if (HAL_I2C_Mem_Read(mHi2c, mDevAddress, mMemAddress, I2C_MEMADD_SIZE_8BIT, buffer, 2, 10) == HAL_OK){
        raw_value = ((uint16_t)buffer[0] << 8) | buffer[1];
        raw_value &= 0x0FFF;
        return true;
    }
    return false ;
}

bool I2CWrapper::Write(uint16_t raw_value) {
    if(mMemAddress == 0)
        return false;

    uint8_t buffer[2];
    buffer[0] = (raw_value >> 8) & 0x0F; // MSB
    buffer[1] = raw_value & 0xFF;        // LSB

    return HAL_I2C_Mem_Write(mHi2c, mDevAddress, mMemAddress,
                             I2C_MEMADD_SIZE_8BIT, buffer, 2, 2) == HAL_OK;
}

void I2CWrapper::SetMemAddress(uint8_t addr){
    mMemAddress = addr;
}

std::shared_ptr<II2CWrapper> MakeII2CWrapper(I2C_HandleTypeDef* hi2c, uint16_t devAddress) {
    return std::make_shared<I2CWrapper>(hi2c, devAddress);
}
