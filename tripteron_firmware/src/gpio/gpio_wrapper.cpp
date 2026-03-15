
#include "gpio_wrapper.h"
#include "gpio_input_interface.h"
#include "gpio_output_interface.h"
#include "utils.h"
#include "stm32f4xx_hal.h"

#include <memory>

std::shared_ptr<IGpioWrapper> MakeIGpioWrapper(GpioMode mode, GPIO_TypeDef *gpiox, uint16_t pin) {
    return mode == GpioMode::OUTPUT ?
        std::static_pointer_cast<IGpioWrapper>(MakeIGpioOutput(gpiox, pin)) :
        std::static_pointer_cast<IGpioWrapper>(MakeIGpioInput(gpiox, pin));
}

