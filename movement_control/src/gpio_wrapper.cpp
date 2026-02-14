#include "gpio_wrapper.h"
#include "stm32f4xx_hal.h"

void GPIO::Set() {
    HAL_GPIO_WritePin(mGPIOx, mPin, GPIO_PIN_SET);
}

void GPIO::Reset() {
    HAL_GPIO_WritePin(mGPIOx, mPin, GPIO_PIN_RESET);
}

void GPIO::Toggle() {
    HAL_GPIO_TogglePin(mGPIOx, mPin);
}

