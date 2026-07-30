#pragma once

#include "usb_cdc_interface.h"

class UsbCdc : public IUsbCdc {
public:
    UsbCdc() = default;

    bool Write(uint8_t* data, uint16_t len) override;
    void RegisterRxCallback(CdcRxCallback callback) override;
    void HandleRxEvent(uint8_t* data, uint16_t len) override;

private:
    CdcRxCallback mRxCallback;
};
