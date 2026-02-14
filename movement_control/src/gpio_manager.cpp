#include "gpio_manager.h"
#include "gpio.h" // Tiene que ser exclusivo de este .cpp
#include "gpio_wrapper.h"

using enum GPIOId;

GPIOManager& GPIOManager::GetInstance(){
    static GPIOManager instance;
    return instance;
}

void GPIOManager::InitGPIOs() {
    // Aquí se inicializan los GPIOs y se agregan a la lista
    mGPIOs.push_back(std::make_shared<GPIO>(GPIOA, DIR_Pin, GPIOA1));
}

std::shared_ptr<GPIO> GPIOManager::GetGPIO(GPIOId id) {
    for (const auto& gpio : mGPIOs) {
        if (gpio->mId == id) return gpio;
    }
    return nullptr; // No se encontró el GPIO con el ID solicitado
}
