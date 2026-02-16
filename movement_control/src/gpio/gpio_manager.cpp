#include "gpio_manager.h"
#include "gpio.h" // Tiene que ser exclusivo de este .cpp
#include "gpio_wrapper.h"
#include "main.h"
#include "stm32f407xx.h"
#include "utils.h"
#include <cassert>

using enum GPIOId;

GPIOManager& GPIOManager::GetInstance(){
    static GPIOManager instance;
    return instance;
}

bool GPIOManager::InitGPIOs() {
    // Aquí se inicializan los GPIOs y se agregan a la lista
    MX_GPIO_Init();
    return (
        AddGPIO(GPIOA1, GPIOA, DIR_Pin) &&
        AddGPIO(GPIOA2, GPIOA, EN_Pin)
    );
}

bool GPIOManager::AddGPIO(GPIOId id, GPIO_TypeDef* gpiox, uint16_t pin) {
    auto [it, inserted] = mGPIOsMap.emplace(id, std::make_shared<GPIO>(gpiox, pin));
    return inserted; // Devuelve true si se insertó correctamente, false si ya existía un GPIO con ese ID
}

std::shared_ptr<GPIO> GPIOManager::GetGPIO(GPIOId id) {
    auto it = mGPIOsMap.find(id);
    if (it != mGPIOsMap.end()) {
        return it->second;
    }
    return nullptr; // No se encontró el GPIO con el ID solicitado
}
