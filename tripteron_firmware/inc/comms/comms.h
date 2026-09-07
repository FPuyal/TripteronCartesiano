#pragma once

#include "comms_interface.h"

class Comms : public IComms {
public:
    Comms(std::shared_ptr<IUsbCdc> usb);

    void RegisterCommandCallback(CommandCallback callback) override;
    bool SendData(uint8_t* data, uint16_t len) override;

private:
    void OnBytesReceived(uint8_t* data, uint16_t len);

    static constexpr uint16_t kMaxRawCommandLength = 256;

    uint8_t mRawCommand[kMaxRawCommandLength];
    uint8_t mRawCommandLength = 0;
    bool mCommandFinish = false;

    CommandCallback mCommandCallback;

    std::shared_ptr<IUsbCdc> mUsb;
};
