#include "hardware_manager.h"

// STM HAL includes
#include "gpio.h"
#include "tim.h"
#include "usart.h"
#include "i2c.h"
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
    MX_TIM1_Init();
    MX_TIM2_Init();
    MX_I2C1_Init();
    MX_I2C2_Init();
    MX_I2C3_Init();
    MX_USART2_UART_Init();
    HAL_Delay(500);

    mTimers[TimerId::Tim1] = MakeITimer(&htim1);
    mTimers[TimerId::Tim2] = MakeITimer(&htim2);

    mEncoders[EncoderId::XEncoder] = MakeIEncoder(MakeII2CWrapper(&hi2c1, 0x6C));
    mEncoders[EncoderId::YEncoder] = MakeIEncoder(MakeII2CWrapper(&hi2c2, 0x6C));
    mEncoders[EncoderId::ZEncoder] = MakeIEncoder(MakeII2CWrapper(&hi2c3, 0x6C));

    mEndStops[EndStopId::XEnd] = MakeIGpioInput(GPIOA, GPIO_PIN_4);

    mTmcs[TmcId::XTmc] = MakeITmc(
            MakeIGpioOutput(GPIOA, GPIO_PIN_0),
            MakeIGpioOutput(GPIOA, GPIO_PIN_1),
            MakeIGpioOutput(GPIOA, GPIO_PIN_2),
            MakeIUart(&huart2),
            0x00,
            4
    );

    mTimers[TimerId::Tim1]->Stop();
    mTimers[TimerId::Tim2]->Stop();
}

std::shared_ptr<ITimer> HardwareManager::GetTimer(TimerId id) {
    return mTimers[id];
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
