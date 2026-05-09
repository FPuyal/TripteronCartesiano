#pragma once

#include "uart_manager_interface.h"
#include "resource_manager.h"

class UartManager : public ResourceManager<IUartManager, IUart, UartId, UartInfo> {
public:
    UartManager(std::vector<UartInfo> uartInfo) : ResourceManager() {
        InitResources(uartInfo);
    }
    std::shared_ptr<IUart> GetUart(UartId id) override {
        return GetResource(id);
    }
protected:
    std::shared_ptr<IUart> Make(UartInfo uartInfo) override {
        return MakeIUart(uartInfo.huart);
    }
};
