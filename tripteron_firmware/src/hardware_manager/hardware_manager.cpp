#include "hardware_manager.h"
#include "utils.h"

#include "gpio.h" // Tiene que ser exclusivo de este .cpp
#include "tim.h" // Tiene que ser exclusivo de este .cpp
#include "i2c.h" // Tiene que ser exclusivo de este .cpp
#include <vector>

#define AS5600_ADDR (0x36 << 1)

extern "C" void SystemClock_Config(void);

void HardwareManager::InitHardware() {
    SystemClock_Config();

    MX_GPIO_Init();
    MX_TIM2_Init();
    MX_I2C2_Init();

    // Configuración del HW
    std::vector<GpioInfo> gpiosInfo = {
        {GpioId::EN_TMCX, GPIOA, GPIO_PIN_0},
        {GpioId::DIR_TMCX, GPIOA, GPIO_PIN_1}
    };

    std::vector<TimerInfo> timersInfo = {
        {TimerId::STEP_TMCX, &htim2, TIM_CHANNEL_1}
    };

    mGpioManager = MakeIGpioManager(gpiosInfo);
    mTimerManager = MakeITimerManager(timersInfo);

    // Configuración de los TMCs
    std::vector<TmcInfo> tmcsInfo = {
        {TmcId::TMCX,
            mTimerManager->GetTimer(TimerId::STEP_TMCX),
            mGpioManager->GetGpio(GpioId::EN_TMCX),
            mGpioManager->GetGpio(GpioId::DIR_TMCX)}
    };

    mTmcManager = MakeITmcManager(tmcsInfo);

    std::vector<I2cInfo> i2csInfos = {
        {I2cId::I2CX, &hi2c2, AS5600_ADDR}
    };

    mI2CManager = MakeII2CManager(i2csInfos);

    std::vector<EncoderInfo> encodersInfo = {
        {EncoderId::EncoderX,
        mI2CManager->GetI2C(I2cId::I2CX)}
    };

    mEncoderManager = MakeIEncoderManager(encodersInfo);

}

std::map<TmcId, std::shared_ptr<ITmc>> HardwareManager::GetTmcs() {
    std::map<TmcId, std::shared_ptr<ITmc>> tmcs;
    for(int i = 0; i < static_cast<int>(TmcId::COUNT); i++) {
        auto tmc = mTmcManager->GetTmc(static_cast<TmcId>(i));
        if (!tmc) {
            // error al obtener el TMC
            break;
        }
        tmcs[static_cast<TmcId>(i)] = tmc;
    }
    return tmcs;
}

std::map<EncoderId, std::shared_ptr<IEncoder>> HardwareManager::GetEncoders() {
    std::map<EncoderId, std::shared_ptr<IEncoder>> encoders;
    for(int i = 0; i < static_cast<int>(EncoderId::COUNT); i++) {
        auto enc = mEncoderManager->GetEncoder(static_cast<EncoderId>(i));
        if (!enc) {
            // error al obtener el TMC
            break;
        }
        encoders[static_cast<EncoderId>(i)] = enc;
    }
    return encoders;
}

std::shared_ptr<IHardwareManager> MakeIHardwareManager() {
    return std::make_shared<HardwareManager>();
}
