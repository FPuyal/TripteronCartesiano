#include "uart.h"
#include <memory>

bool Uart::WriteData(uint8_t* data) {
    bool result = HAL_UART_Transmit(mHuart, data, 8, 1000) == HAL_OK;
    HAL_Delay(10);
    return result;
}

bool Uart::ReadData(uint8_t* data) {
    // return HAL_UART_Receive(mHuart, (uint8_t*)this, sizeof(this), 1000) == HAL_OK;
    return false;
}

std::shared_ptr<IUart> MakeIUart(UART_HandleTypeDef *huart){
    return std::make_shared<Uart>(huart);
}
