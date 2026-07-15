#include "gpio_input.h"
#include "stm32f4xx_hal_gpio.h"

bool GpioInput::Read() {
    return HAL_GPIO_ReadPin(mGPIOx, mPin) == GPIO_PIN_SET;
}

std::shared_ptr<IGpioInput> MakeIGpioInput(GPIO_TypeDef *gpiox, uint16_t pin) {
    return std::make_shared<GpioInput>(gpiox, pin);
}

