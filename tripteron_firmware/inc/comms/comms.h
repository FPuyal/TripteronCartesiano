#pragma once

#include "comms_interface.h"

class Comms : public IComms {
public:
    Comms(std::shared_ptr<IUsbCdc> usb);

    void RegisterCommandCallback(CommandCallback callback) override;
    bool SendTelemetry(const TelemetryPayload& telemetry) override;
    bool SendEnd() override;

private:
    enum class RxState {
        WaitStart,
        ReadId,
        ReadPayload,
    };

    static constexpr uint16_t MaxPayload = 64;

    void OnBytesReceived(uint8_t* data, uint16_t len);
    void ParseByte(uint8_t byte);
    void DispatchFrame();

    std::shared_ptr<IUsbCdc> mUsb;
    CommandCallback mCommandCallback;

    RxState mState = RxState::WaitStart;
    uint8_t mId = 0;
    uint8_t mPayloadLen = 0;
    uint8_t mPayload[MaxPayload];
    uint8_t mPayloadIndex = 0;

    uint8_t mTxBuffer[MaxPayload + 4];
};
