#include "comms.h"

#include <cstring>

namespace {
constexpr uint8_t InvalidId = 0xFF;

uint8_t PayloadSize(uint8_t id) {
    switch (static_cast<MessageId>(id)) {
        case MessageId::Home: return 0;
        case MessageId::Stop: return 0;
        case MessageId::Move: return sizeof(MovePayload);
        default:              return InvalidId;
    }
}
}  // namespace

Comms::Comms(std::shared_ptr<IUsbCdc> usb) : mUsb(usb) {
    mUsb->RegisterRxCallback([this](uint8_t* data, uint16_t len) {
        OnBytesReceived(data, len);
    });
}

void Comms::RegisterCommandCallback(CommandCallback callback) {
    mCommandCallback = callback;
}

void Comms::OnBytesReceived(uint8_t* data, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        ParseByte(data[i]);
    }
}

void Comms::ParseByte(uint8_t byte) {
    switch (mState) {
        case RxState::WaitStart:
            if (byte == FrameStart)
                mState = RxState::ReadId;
            break;

        case RxState::ReadId: {
            mId = byte;
            mPayloadLen = PayloadSize(byte);
            mPayloadIndex = 0;
            if (mPayloadLen == InvalidId) {
                mState = RxState::WaitStart;
            } else if (mPayloadLen == 0) {
                DispatchFrame();
                mState = RxState::WaitStart;
            } else {
                mState = RxState::ReadPayload;
            }
            break;
        }

        case RxState::ReadPayload:
            mPayload[mPayloadIndex++] = byte;
            if (mPayloadIndex >= mPayloadLen) {
                DispatchFrame();
                mState = RxState::WaitStart;
            }
            break;
    }
}

void Comms::DispatchFrame() {
    if (!mCommandCallback)
        return;

    Command command{};
    command.id = static_cast<MessageId>(mId);

    if (command.id == MessageId::Move)
        memcpy(&command.move, mPayload, sizeof(MovePayload));

    mCommandCallback(command);
}

bool Comms::SendTelemetry(const TelemetryPayload& telemetry) {
    uint8_t len = sizeof(TelemetryPayload);
    mTxBuffer[0] = FrameStart;
    mTxBuffer[1] = static_cast<uint8_t>(MessageId::Telemetry);
    memcpy(&mTxBuffer[2], &telemetry, len);
    return mUsb->Write(mTxBuffer, 2 + len);
}

bool Comms::SendEnd() {
    mTxBuffer[0] = FrameStart;
    mTxBuffer[1] = static_cast<uint8_t>(MessageId::End);
    return mUsb->Write(mTxBuffer, 2);
}

std::shared_ptr<IComms> MakeIComms(std::shared_ptr<IUsbCdc> usb) {
    return std::make_shared<Comms>(usb);
}
