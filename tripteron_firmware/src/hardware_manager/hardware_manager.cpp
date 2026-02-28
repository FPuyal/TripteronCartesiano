#include "hardware_manager_interface.h"
#include "hardware_manager.h"
#include "utils.h"

#include "gpio.h" // Tiene que ser exclusivo de este .cpp
#include "tim.h" // Tiene que ser exclusivo de este .cpp

extern "C" void SystemClock_Config(void);

void HardwareManager::InitHardware() {
    SystemClock_Config();

    MX_GPIO_Init();
    MX_TIM2_Init();

    // Configuración del HW
    std::vector<GpioInfo> gpioInfos = {
        {GpioId::EN_TMCX, GPIOA, GPIO_PIN_0},
        {GpioId::DIR_TMCX, GPIOA, GPIO_PIN_1}
    };

    std::vector<TimerInfo> timersInfos = {
        {TimerId::STEP_TMCX, &htim2, TIM_CHANNEL_1}
    };

    mGpioManager = MakeIGpioManager(gpioInfos);
    mTimerManager = MakeITimerManager(timersInfos);

    // Configuración de los TMCs
    std::vector<TmcInfo> tmcInfos = {
        {TmcId::TMCX,
            mTimerManager->GetTimer(TimerId::STEP_TMCX),
            mGpioManager->GetGpio(GpioId::EN_TMCX),
            mGpioManager->GetGpio(GpioId::DIR_TMCX)}
    };

    mTmcManager = MakeITmcManager(tmcInfos);
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

std::shared_ptr<IHardwareManager> MakeIHardwareManager() {
    return std::make_shared<HardwareManager>();
}
