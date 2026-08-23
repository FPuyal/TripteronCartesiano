#include "encoder.h"

#include "stm32f4xx_hal.h"
#include <cstdint>
#include <memory>

static constexpr uint8_t kAS5600RawAngle = 0x0C;

Encoder::Encoder(std::shared_ptr<II2CWrapper> i2cWrapper) : mI2cWrapper(i2cWrapper) {
    mI2cWrapper->SetMemAddress(kAS5600RawAngle);

    mI2cWrapper->SetReadCallback([this](uint16_t raw_value) {
        mRawValue = raw_value;
        mDataReady = true;
    });

    mI2cWrapper->SetErrorCallback([this]() {
        mDataReady = false;
    });
}

bool Encoder::SetOffset() {
    mDataReady = false;
    if (!mI2cWrapper->ReadIT())
        return false;

    uint32_t start = HAL_GetTick();
    while (!mDataReady) {
        if (HAL_GetTick() - start > kOffsetTimeoutMs)
            return false;
    }

    mDataReady = false;
    mHomeRawValue = mRawValue;
    return true;
}

bool Encoder::ReadAngle(float& angle) {
    bool ready = mDataReady;

    if (ready) {
        mDataReady = false;
        int32_t delta = (int32_t)mRawValue - (int32_t)mHomeRawValue;
        delta = (delta + 4096) % 4096;
        angle = delta * 360.0f / 4096.0f;
        if (angle > 180.0f)
            angle -= 360.0f;
    }

    mI2cWrapper->ReadIT();   // siempre relanza, haya o no dato
    return ready;
}

std::shared_ptr<IEncoder> MakeIEncoder(std::shared_ptr<II2CWrapper> i2cWrapper) {
    return std::make_shared<Encoder>(i2cWrapper);
}
