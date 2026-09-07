#include "i2c_wrapper.h"

I2CWrapper* I2CWrapper::sInstanceI2C1 = nullptr;
I2CWrapper* I2CWrapper::sInstanceI2C2 = nullptr;
I2CWrapper* I2CWrapper::sInstanceI2C3 = nullptr;

extern "C" void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef* hi2c) {
    I2CWrapper* instance = I2CWrapper::GetInstance(hi2c);
    if (instance) instance->OnRxComplete();
}

extern "C" void HAL_I2C_ErrorCallback(I2C_HandleTypeDef* hi2c) {
    I2CWrapper* instance = I2CWrapper::GetInstance(hi2c);
    if (instance) instance->OnError();
}

I2CWrapper::I2CWrapper(I2C_HandleTypeDef* hi2c, uint16_t devAddress) :
    mHi2c(hi2c), mDevAddress(devAddress), mMemAddress(0) {
        if (hi2c->Instance == I2C1) sInstanceI2C1 = this;
        else if (hi2c->Instance == I2C2) sInstanceI2C2 = this;
        else if (hi2c->Instance == I2C3) sInstanceI2C3 = this;
}

bool I2CWrapper::ReadIT() {
    if(!mMemAddress)
        return false;

    return HAL_I2C_Mem_Read_IT(
        mHi2c,
        mDevAddress,
        mMemAddress,
        I2C_MEMADD_SIZE_8BIT,
        mRxBuf,
        2) == HAL_OK;
}

void I2CWrapper::OnRxComplete() {
    uint16_t raw_value = ((uint16_t)mRxBuf[0] << 8) | mRxBuf[1];
    raw_value &= 0x0FFF;
    mErrorCount = 0;
    if (mReadCallback) mReadCallback(raw_value);
}

void I2CWrapper::OnError() {
    if (++mErrorCount >= kMaxRetries) {
        mErrorCount = 0;
        if (mErrorCallback) mErrorCallback();
    }
    else {
        ReadIT();
    }
}

I2CWrapper* I2CWrapper::GetInstance(I2C_HandleTypeDef* hi2c) {
    if (hi2c->Instance == I2C1) return sInstanceI2C1;
    if (hi2c->Instance == I2C2) return sInstanceI2C2;
    if (hi2c->Instance == I2C3) return sInstanceI2C3;
    return nullptr;
}

void I2CWrapper::SetMemAddress(uint8_t addr){
    mMemAddress = addr;
}

void I2CWrapper::SetReadCallback(ReadCallback callback) {
    mReadCallback = callback;
}

void I2CWrapper::SetErrorCallback(ErrorCallback callback) {
    mErrorCallback = callback;
}

std::shared_ptr<II2CWrapper> MakeII2CWrapper(I2C_HandleTypeDef* hi2c, uint16_t devAddress) {
    return std::make_shared<I2CWrapper>(hi2c, devAddress);
}
