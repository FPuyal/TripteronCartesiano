#include "uart.h"
#include <memory>

bool Uart::WriteData(uint8_t* data) {
    return HAL_UART_Transmit(mHuart, data, 8, 1000) == HAL_OK;
}

bool Uart::ReadData(uint8_t* data) {
    return HAL_UART_Receive(mHuart, data, 8, 1000) == HAL_OK;
}

std::shared_ptr<IUart> MakeIUart(UART_HandleTypeDef *huart){
    return std::make_shared<Uart>(huart);
}
