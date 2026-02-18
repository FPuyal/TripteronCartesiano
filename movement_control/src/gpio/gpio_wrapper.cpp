#include "gpio_wrapper.h"
#include "stm32f4xx_hal.h"
#include <memory>

void GpioWrapper::Set() {
    HAL_GPIO_WritePin(mGPIOx, mPin, GPIO_PIN_SET);
}

void GpioWrapper::Reset() {
    HAL_GPIO_WritePin(mGPIOx, mPin, GPIO_PIN_RESET);
}

void GpioWrapper::Toggle() {
    HAL_GPIO_TogglePin(mGPIOx, mPin);
}

std::shared_ptr<IGpioWrapper> MakeIGpioWrapper(GPIO_TypeDef *gpiox, uint16_t pin) {
    return std::make_shared<GpioWrapper>(gpiox, pin);
}

