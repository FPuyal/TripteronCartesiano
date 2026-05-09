#pragma once

#include "uart_interface.h"
#include "utils.h"

#include <memory>
#include <vector>

class IUartManager {
public:
    virtual ~IUartManager() = default;
    virtual std::shared_ptr<IUart> GetUart(UartId id) = 0;
};

std::shared_ptr<IUartManager> MakeIUartManager(std::vector<UartInfo> uartInfos);
