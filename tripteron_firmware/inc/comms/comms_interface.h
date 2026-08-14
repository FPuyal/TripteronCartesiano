#pragma once

#include "usb_cdc_interface.h"

#include <cstdint>
#include <functional>
#include <memory>

using CommandCallback = std::function<void(uint8_t*, uint16_t)>;

class IComms {
public:
    virtual ~IComms() = default;

    virtual void RegisterCommandCallback(CommandCallback callback) = 0;
    virtual bool SendData(uint8_t* data, uint16_t len) = 0;
};

std::shared_ptr<IComms> MakeIComms(std::shared_ptr<IUsbCdc> usb);
