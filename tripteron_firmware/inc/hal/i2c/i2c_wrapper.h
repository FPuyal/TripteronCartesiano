#pragma once

#include "i2c_wrapper_interface.h"

class I2CWrapper : public II2CWrapper {
public:
    I2CWrapper(I2C_HandleTypeDef* hi2c, uint16_t devAddress);
    bool ReadIT() override;
    void OnRxComplete() override;
    void OnError() override;
    void SetMemAddress(uint8_t addr) override;
    void SetReadCallback(ReadCallback callback) override;
    void SetErrorCallback(ErrorCallback callback) override;

    static I2CWrapper* GetInstance(I2C_HandleTypeDef* hi2c);

private:
    static I2CWrapper* sInstanceI2C1;
    static I2CWrapper* sInstanceI2C2;
    static I2CWrapper* sInstanceI2C3;
    static constexpr uint8_t kMaxRetries = 3;

    I2C_HandleTypeDef* mHi2c;
    uint16_t mDevAddress;
    uint8_t mMemAddress;

    uint8_t mRxBuf[2];
    ReadCallback mReadCallback;
    ErrorCallback mErrorCallback;
    uint8_t mErrorCount = 0;
};
