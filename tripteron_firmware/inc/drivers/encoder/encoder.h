#pragma once

#include "encoder_interface.h"

class Encoder : public IEncoder {
public:
    Encoder(std::shared_ptr<II2CWrapper> i2cWrapper);
    bool SetOffset() override;
    bool RequestUpdate() override;
    bool ReadAngle(float& angle) override;

private:
    static constexpr uint32_t kOffsetTimeoutMs = 50;
    static constexpr uint32_t kTimeMarkThreshold = 3; // Valor máximo de antiguedad de la lectura antes de considerarla inválida

    uint16_t mHomeRawValue = 0;
    volatile uint16_t mRawValue = 0;
    volatile uint8_t mTimeMark =  255;

    std::shared_ptr<II2CWrapper> mI2cWrapper;
};
