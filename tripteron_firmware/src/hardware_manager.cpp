#include "hardware_manager.h"

// STM HAL includes
#include "gpio.h"
#include "stm32f407xx.h"
#include "tim.h"
#include "usart.h"
#include "i2c.h"
#include "usb_device.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

// Resource interfaces includes
#include "timer_interface.h"
#include "gpio_output_interface.h"
#include "uart_interface.h"
#include "tmc_interface.h"
#include "gpio_input_interface.h"
#include "usb_cdc_interface.h"

#include <memory>

extern IUsbCdc* usbCdcInstance;
extern "C" volatile uint8_t UsbCdc_HostConnected;
extern "C" volatile uint8_t UsbCdc_RxReady;

extern "C" void SystemClock_Config(void);
static void I2C_BusRecovery(GPIO_TypeDef* sclPort, uint16_t sclPin, GPIO_TypeDef* sdaPort, uint16_t sdaPin);

void HardwareManager::InitHardware() {
    SystemClock_Config();

    MX_GPIO_Init();

    MX_TIM1_Init();
    MX_TIM2_Init();

    I2C_BusRecovery(GPIOB, GPIO_PIN_8, GPIOB, GPIO_PIN_9);
    I2C_BusRecovery(GPIOB, GPIO_PIN_10, GPIOB, GPIO_PIN_3);
    I2C_BusRecovery(GPIOA, GPIO_PIN_8, GPIOC, GPIO_PIN_9);

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
            0x03,
            4
    );
    mTmcs[TmcId::YTmc] = MakeITmc(
            MakeIGpioOutput(GPIOB, GPIO_PIN_6),
            MakeIGpioOutput(GPIOB, GPIO_PIN_5),
            MakeIGpioOutput(GPIOB, GPIO_PIN_4),
            MakeIUart(&huart2),
            0x02,
            4
    );
    mTmcs[TmcId::ZTmc] = MakeITmc(
            MakeIGpioOutput(GPIOD, GPIO_PIN_6),
            MakeIGpioOutput(GPIOD, GPIO_PIN_5),
            MakeIGpioOutput(GPIOD, GPIO_PIN_4),
            MakeIUart(&huart2),
            0x01,
            1
    );

    mTimers[TimerId::Tim1]->Stop();
    mTimers[TimerId::Tim2]->Stop();

    mUsbCdc = MakeIUsbCdc();
    usbCdcInstance = mUsbCdc.get();

    MX_USB_DEVICE_Init();

    while (!UsbCdc_HostConnected) {}
    while (!UsbCdc_RxReady) {}
}

static void I2C_BusRecovery(GPIO_TypeDef* sclPort, uint16_t sclPin, GPIO_TypeDef* sdaPort, uint16_t sdaPin) {
    GPIO_InitTypeDef g = {0};
    g.Mode  = GPIO_MODE_OUTPUT_OD;
    g.Pull  = GPIO_PULLUP;
    g.Speed = GPIO_SPEED_FREQ_LOW;

    g.Pin = sclPin;  HAL_GPIO_Init(sclPort, &g);
    g.Pin = sdaPin;  HAL_GPIO_Init(sdaPort, &g);

    HAL_GPIO_WritePin(sclPort, sclPin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(sdaPort, sdaPin, GPIO_PIN_SET);

    for (int i = 0; i < 9; i++) {
        HAL_GPIO_WritePin(sclPort, sclPin, GPIO_PIN_RESET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(sclPort, sclPin, GPIO_PIN_SET);
        HAL_Delay(1);
        if (HAL_GPIO_ReadPin(sdaPort, sdaPin) == GPIO_PIN_SET) break;
    }

    // STOP manual
    HAL_GPIO_WritePin(sdaPort, sdaPin, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(sclPort, sclPin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(sdaPort, sdaPin, GPIO_PIN_SET);
    HAL_Delay(1);
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

std::shared_ptr<IUsbCdc> HardwareManager::GetUsbCdc() {
    return mUsbCdc;
}

std::shared_ptr<IHardwareManager> MakeIHardwareManager() {
    return std::make_shared<HardwareManager>();
}
