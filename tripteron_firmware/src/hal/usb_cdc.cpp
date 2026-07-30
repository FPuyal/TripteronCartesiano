#include "usb_cdc.h"

#include "usbd_cdc_if.h"

bool UsbCdc::Write(uint8_t* data, uint16_t len) {
    return CDC_Transmit_FS(data, len) == USBD_OK;
}

void UsbCdc::RegisterRxCallback(CdcRxCallback callback) {
    mRxCallback = callback;
}

void UsbCdc::HandleRxEvent(uint8_t* data, uint16_t len) {
    if (mRxCallback) mRxCallback(data, len);
}

std::shared_ptr<IUsbCdc> MakeIUsbCdc() {
    return std::make_shared<UsbCdc>();
}
