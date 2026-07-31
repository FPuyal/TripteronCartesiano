#pragma once

#include "usb_cdc_interface.h"
#include "protocol.h"

#include <functional>
#include <memory>

using CommandCallback = std::function<void(const Command& command)>;

class IComms {
public:
    virtual ~IComms() = default;

    virtual void RegisterCommandCallback(CommandCallback callback) = 0;
    virtual bool SendTelemetry(const TelemetryPayload& telemetry) = 0;
    virtual bool SendEnd() = 0;
};

std::shared_ptr<IComms> MakeIComms(std::shared_ptr<IUsbCdc> usb);
