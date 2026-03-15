#include "gpio_output.h"
#include "stm32f4xx_hal_gpio.h"

void GpioOutput::Set() {
    HAL_GPIO_WritePin(mGPIOx, mPin, GPIO_PIN_SET);
}

void GpioOutput::Reset() {
    HAL_GPIO_WritePin(mGPIOx, mPin, GPIO_PIN_RESET);
}

void GpioOutput::Toggle() {
    HAL_GPIO_TogglePin(mGPIOx, mPin);
}

std::shared_ptr<IGpioOutput> MakeIGpioOutput(GPIO_TypeDef *gpiox, uint16_t pin) {
    return std::make_shared<GpioOutput>(gpiox, pin);
}
