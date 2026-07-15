#include "hardware_manager.h"

// STM HAL includes
#include "gpio.h"
#include "stm32f401xe.h"
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

    mEndStops[EndStopId::XEnd] = MakeIGpioInput(GPIOC, GPIO_PIN_13);
    mEndStops[EndStopId::YEnd] = MakeIGpioInput(GPIOC, GPIO_PIN_14);
    mEndStops[EndStopId::ZEnd] = MakeIGpioInput(GPIOC, GPIO_PIN_15);

    mTmcs[TmcId::XTmc] = MakeITmc(
            MakeIGpioOutput(GPIOE, GPIO_PIN_6),
            MakeIGpioOutput(GPIOE, GPIO_PIN_5),
            MakeIGpioOutput(GPIOE, GPIO_PIN_4),
            MakeIUart(&huart2),
            0x00,
            4
    );
    mTmcs[TmcId::YTmc] = MakeITmc(
            MakeIGpioOutput(GPIOB, GPIO_PIN_6),
            MakeIGpioOutput(GPIOB, GPIO_PIN_5),
            MakeIGpioOutput(GPIOB, GPIO_PIN_4),
            MakeIUart(&huart2),
            0x01,
            4
    );
    mTmcs[TmcId::ZTmc] = MakeITmc(
            MakeIGpioOutput(GPIOD, GPIO_PIN_6),
            MakeIGpioOutput(GPIOD, GPIO_PIN_5),
            MakeIGpioOutput(GPIOD, GPIO_PIN_4),
            MakeIUart(&huart2),
            0x02,
            1
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
