#include "stm32f4xx_hal.h"
#include "gpio_manager.h"

GpioManager::GpioManager(std::vector<GpioInfo> gpioInfos) {
    if (!InitGpios(gpioInfos)) {
        // Manejar el error de inicialización
    }
}

bool GpioManager::InitGpios(std::vector<GpioInfo> gpioInfos) {
    // Aquí se inicializan los GPIOs y se agregan a la lista
    for (const auto& gpioInfo : gpioInfos) {
        if (!SetGpio(gpioInfo)) {
            return false; // Si alguno falla, se devuelve false
        }
    }
    return true;
}

bool GpioManager::SetGpio(GpioInfo gpioInfo) {
    auto [it, inserted] = mGpiosMap.emplace(gpioInfo.id, MakeIGpioWrapper(gpioInfo.gpiox, gpioInfo.pin));
    return inserted; // Devuelve true si se insertó correctamente, false si ya existía un GPIO con ese ID
}

std::shared_ptr<IGpioWrapper> GpioManager::GetGpio(GpioId id) {
    auto it = mGpiosMap.find(id);
    if (it != mGpiosMap.end()) {
        return it->second;
    }
    return nullptr; // No se encontró el GPIO con el ID solicitado
}

std::shared_ptr<IGpioManager> MakeIGpioManager(std::vector<GpioInfo> gpioInfos) {
    return std::make_shared<GpioManager>(gpioInfos);
}
