#include "encoder.h"

#include "stm32f4xx_hal.h"
#include <cstdint>
#include <memory>

static constexpr uint8_t kAS5600RawAngle = 0x0C;

Encoder::Encoder(std::shared_ptr<II2CWrapper> i2cWrapper) : mI2cWrapper(i2cWrapper) {
    mI2cWrapper->SetMemAddress(kAS5600RawAngle);

    mI2cWrapper->SetReadCallback([this](uint16_t raw_value) {
        mRawValue = raw_value;
        mReadPending = false;
        mDataReady = true;
    });

    mI2cWrapper->SetErrorCallback([this]() {

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

    mHomeRawValue = mRawValue;
    return true;
}

bool Encoder::RequestUpdate() {
    if(mReadPending)
        return false; // Error. mReadPending debe ser false al solicitar una nueva lectura. Esto indica que la lectura anterior no se completó.

    mReadPending = mI2cWrapper->ReadIT();
    return mReadPending; // Devuelve true si la solicitud de lectura se inició correctamente, false si hubo un error.
}

bool Encoder::ReadAngle(float& angle) {
    int32_t delta = (int32_t)mRawValue - (int32_t)mHomeRawValue;
    delta = (delta + 4096) % 4096;
    angle = delta * 360.0f / 4096.0f;
    if (angle > 180.0f)
        angle -= 360.0f;

    mDataReady = false;

    return true;
}

void Encoder::Reset() {
    mReadPending = false;
    mDataReady = false;
    mRawValue = 0;
    mHomeRawValue = 0;
}

std::shared_ptr<IEncoder> MakeIEncoder(std::shared_ptr<II2CWrapper> i2cWrapper) {
    return std::make_shared<Encoder>(i2cWrapper);
}
