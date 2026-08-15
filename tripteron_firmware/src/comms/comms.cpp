#include "comms.h"

#include <cstring>
#include <ranges>

Comms::Comms(std::shared_ptr<IUsbCdc> usb) : mUsb(usb) {
    mUsb->RegisterRxCallback([this](uint8_t* data, uint16_t len) {
        OnBytesReceived(data, len);
    });
}

void Comms::RegisterCommandCallback(CommandCallback callback) {
    mCommandCallback = callback;
}

bool Comms::SendData(uint8_t* data, uint16_t len) {
    return mUsb->Write(data, len);
}

void Comms::OnBytesReceived(uint8_t* data, uint16_t len) {
    for(int i = 0; i < len; i++) {
        mRawCommand[mRawCommandLength++] = data[i];
        if(data[i] == '\0') {
            mCommandFinish = true;
            break;
        }
        if(mRawCommandLength >= mMaxRawCommandLength) {
            mRawCommandLength = 0;
            break;
        }
    }

    if(mCommandFinish && mCommandCallback) {
        mCommandCallback(mRawCommand, mRawCommandLength);
        mCommandFinish = false;
        mRawCommandLength = 0;
    }
}

std::shared_ptr<IComms> MakeIComms(std::shared_ptr<IUsbCdc> usb) {
    return std::make_shared<Comms>(usb);
}
