#pragma once

#include "encoder_interface.h"

class Encoder : public IEncoder {
public:
    Encoder(std::shared_ptr<II2CWrapper> i2cWrapper);
    bool SetOffset() override;
    bool RequestUpdate() override;
    bool IsDataReady() const override { return mDataReady; }
    bool ReadAngle(float& angle) override;
    void Reset() override;

private:
    void ConfigEncoder();

    bool mConfigured = false; // Indica si el encoder ha sido configurado correctamente

    static constexpr uint32_t kOffsetTimeoutMs = 50;

    volatile bool mReadPending = false;
    volatile bool mDataReady = false;

    uint16_t mHomeRawValue = 0;
    volatile uint16_t mRawValue = 0;

    std::shared_ptr<II2CWrapper> mI2cWrapper;
};
