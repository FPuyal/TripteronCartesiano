#pragma once

#include <cstdint>
#include <functional>
#include <memory>

using CdcRxCallback = std::function<void(uint8_t* data, uint16_t len)>;

class IUsbCdc {
public:
    virtual ~IUsbCdc() = default;

    virtual bool Write(uint8_t* data, uint16_t len) = 0;
    virtual void RegisterRxCallback(CdcRxCallback callback) = 0;
    virtual void HandleRxEvent(uint8_t* data, uint16_t len) = 0;
};

std::shared_ptr<IUsbCdc> MakeIUsbCdc();
