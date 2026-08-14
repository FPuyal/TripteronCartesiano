#pragma once

#include "comms_interface.h"

class Comms : public IComms {
public:
    Comms(std::shared_ptr<IUsbCdc> usb);

    void RegisterCommandCallback(CommandCallback callback) override;
    bool SendData(uint8_t* data, uint16_t len) override { return false; };

private:
    void OnBytesReceived(uint8_t* data, uint16_t len);

    static constexpr uint16_t mMaxRawCommandLength = 256;

    uint8_t mRawCommand[mMaxRawCommandLength];
    uint8_t mRawCommandLength = 0;
    bool mCommandFinish = false;

    CommandCallback mCommandCallback;

    std::shared_ptr<IUsbCdc> mUsb;
};
