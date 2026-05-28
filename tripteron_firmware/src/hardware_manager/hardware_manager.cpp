#include "hardware_manager.h"

// STM HAL includes
#include "gpio.h"
#include "tim.h"
#include "usart.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

// Resource interfaces includes
#include "timer_interface.h"
#include "gpio_output_interface.h"
#include "uart_interface.h"
#include "tmc_interface.h"
#include "gpio_input_interface.h"

#include <memory>

extern "C" void SystemClock_Config(void);

void HardwareManager::InitHardware() {
    SystemClock_Config();

    MX_GPIO_Init();
    MX_TIM2_Init();
    MX_USART2_UART_Init();
    HAL_Delay(500);

    std::shared_ptr<IGpioInput> endStopX = MakeIGpioInput(GPIOA, GPIO_PIN_4);

    std::shared_ptr<ITmc> tmcX = MakeITmc(
            MakeITimer(&htim2, TIM_CHANNEL_1),
            MakeIGpioOutput(GPIOA, GPIO_PIN_1),
            MakeIGpioOutput(GPIOA, GPIO_PIN_2),
            MakeIUart(&huart2),
            0x00,
            2
    );

    mTmcs[TmcId::XTmc] = tmcX;
    mEndStops[EndStopId::XEnd] = endStopX;
}

std::shared_ptr<ITmc> HardwareManager::GetTmc(TmcId id) {
    return mTmcs[id];
}

std::shared_ptr<IEncoder> HardwareManager::GetEncoder(EncoderId id) {
    return mEncoders[id];
}

std::shared_ptr<IGpioInput> HardwareManager::GetEndStop(EndStopId id) {
    return mEndStops[id];
}

std::shared_ptr<IHardwareManager> MakeIHardwareManager() {
    return std::make_shared<HardwareManager>();
}
