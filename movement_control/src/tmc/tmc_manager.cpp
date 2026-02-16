#include "tmc_manager.h"
#include "timer_manager.h"
#include "gpio_manager.h"
#include "utils.h"
#include <memory>

TMCManager& TMCManager::GetInstance() {
    static TMCManager instance;
    return instance;
}

bool TMCManager::InitTMCs() {
    // Aquí se inicializan los TMCs y se agregan a la lista
    TimerManager& timMan = TimerManager::GetInstance();
    GPIOManager& gpioMan = GPIOManager::GetInstance();

    SetupTMCConfig();

    for (const auto& [id, config] : mTMCConfigMap) {
        auto step = timMan.GetTimer(config.stepId);
        auto dir = gpioMan.GetGPIO(config.dirId);
        auto en = gpioMan.GetGPIO(config.enId);
        if (!(step && dir && en) ) {
            return false; // Error al obtener los recursos necesarios
        }
        if (!AddTMC(id, step, dir, en)) {
            return false; // Error al agregar el TMC
        }
    }
    return true;
}

std::shared_ptr<TMC> TMCManager::GetTMC(TMCId id) {
    auto it = mTMCMap.find(id);
    if (it != mTMCMap.end()) {
        return it->second;
    }
    return nullptr;
}

bool TMCManager::AddTMC(TMCId id, std::shared_ptr<Timer> step, std::shared_ptr<GPIO> dir, std::shared_ptr<GPIO> en) {
    if (mTMCMap.find(id) != mTMCMap.end()) {
        return false; // TMC con ese ID ya existe
    }
    mTMCMap[id] = std::make_shared<TMC>(step, dir, en);
    return true;
}

void TMCManager::SetupTMCConfig() {
    // Aquí se configuran los TMCs con sus respectivos timers y GPIOs a través de los ID definidos en utils.h
    mTMCConfigMap[TMCId::TMCX] = {TimerId::GPIOA0, GPIOId::GPIOA1, GPIOId::GPIOA2};
    // mTMCConfigMap[TMCId::TMCY] = {TimerId::GPIOB5, GPIOId::GPIOA1, GPIOId::GPIOA2};
    // mTMCConfigMap[TMCId::TMCZ] = {TimerId::GPIOC6, GPIOId::GPIOA1, GPIOId::GPIOA2};
}
