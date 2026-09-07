#pragma once

#include "uart_interface.h"

class Uart : public IUart {
public:
    Uart(UART_HandleTypeDef *huart) : mHuart(huart) {}
    bool WriteData(uint8_t* data, uint8_t len) override;
    bool ReadData(uint8_t* data, uint8_t len) override;
private:
    UART_HandleTypeDef *mHuart;
};
