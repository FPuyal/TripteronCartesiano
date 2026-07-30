#include "hardware_manager_interface.h"
#include "usb_cdc_interface.h"

#include <cstring>
#include <memory>

int main(){

    auto hardwareManager = MakeIHardwareManager();
    hardwareManager->InitHardware();

    auto usb = hardwareManager->GetUsbCdc();

    static uint8_t rxBuffer[64];
    static volatile uint16_t rxLen = 0;

    usb->RegisterRxCallback([](uint8_t* data, uint16_t len) {
        if(len > sizeof(rxBuffer)) len = sizeof(rxBuffer);
        memcpy(rxBuffer, data, len);
        rxLen = len;
    });

    while(1){
        if(rxLen > 0){
            usb->Write(rxBuffer, rxLen);
            rxLen = 0;
        }
    }

}
