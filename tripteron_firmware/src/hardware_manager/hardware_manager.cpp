#include "hardware_manager.h"
#include "gpio.h" // Tiene que ser exclusivo de este .cpp
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "tim.h" // Tiene que ser exclusivo de este .cpp
#include "i2c.h" // Tiene que ser exclusivo de este .cpp
#include "usart.h"
#include "utils.h"

#include <vector>

#define AS5600_ADDR (0x36 << 1)

extern "C" void SystemClock_Config(void);

void HardwareManager::InitHardware() {
    SystemClock_Config();

    MX_GPIO_Init();
    MX_TIM2_Init();
    MX_I2C1_Init();
    MX_USART2_UART_Init();
    HAL_Delay(500);

    /********************************************************
    *               Configuración del HW
    *********************************************************/
    std::vector<GpioInfo> gpiosInfo = {
        {GpioId::DIR_TMCX, GpioMode::OUTPUT, GPIOA, GPIO_PIN_1},
        {GpioId::EN_TMCX, GpioMode::OUTPUT, GPIOA, GPIO_PIN_2},
        {GpioId::END_STOP_X, GpioMode::INPUT, GPIOA, GPIO_PIN_4}
    };

    std::vector<TimerInfo> timersInfo = {
        {TimerId::STEP_TMCX, &htim2, TIM_CHANNEL_1}
    };

    std::vector<UartInfo> uartsInfo = {
        {UartId::UART2, &huart2}
    };

    mGpioManager = MakeIGpioManager(gpiosInfo);
    mTimerManager = MakeITimerManager(timersInfo);
    mUartManager = MakeIUartManager(uartsInfo);

    /********************************************************
    *               Configuración de los TMCs
    *********************************************************/
    std::vector<TmcInfo> tmcsInfo = {
        {TmcId::TMCX,
            mTimerManager->GetTimer(TimerId::STEP_TMCX),
            mGpioManager->GetGpioOutput(GpioId::EN_TMCX),
            mGpioManager->GetGpioOutput(GpioId::DIR_TMCX),
            mUartManager->GetUart(UartId::UART2),
            0x00,
            2 }
    };

    mTmcManager = MakeITmcManager(tmcsInfo);

    /********************************************************
    *             Configuración de los Encoders
    *********************************************************/

    std::vector<I2cInfo> i2csInfos = {
        {I2cId::I2CX, &hi2c1, AS5600_ADDR}
    };

    mI2CManager = MakeII2CManager(i2csInfos);

    std::vector<EncoderInfo> encodersInfo = {
        {EncoderId::EncoderX,
        mI2CManager->GetI2C(I2cId::I2CX)}
    };

    mEncoderManager = MakeIEncoderManager(encodersInfo);

}

std::shared_ptr<ITmc> HardwareManager::GetTmc(TmcId id) {
    return mTmcManager->GetTmc(id);
}

std::shared_ptr<IEncoder> HardwareManager::GetEncoder(EncoderId id) {
    return mEncoderManager->GetEncoder(id);
}

std::shared_ptr<IGpioInput> HardwareManager::GetEndStop(GpioId id) {
    return mGpioManager->GetGpioInput(id);
}

std::shared_ptr<IHardwareManager> MakeIHardwareManager() {
    return std::make_shared<HardwareManager>();
}
