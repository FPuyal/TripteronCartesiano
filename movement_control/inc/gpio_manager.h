#pragma once

#include <cstdint>
#include <locale>
#include <map>
#include <memory>
#include <set>
#include "gpio_wrapper.h"
#include "utils.h"

class GPIOManager {
public:
    static GPIOManager& GetInstance();
    bool InitGPIOs();
    std::shared_ptr<GPIO> GetGPIO(GPIOId id);
private:
    GPIOManager() = default;
    bool AddGPIO(GPIOId id, GPIO_TypeDef* gpiox, uint16_t pin);
    std::map<GPIOId, std::shared_ptr<GPIO>> mGPIOsMap;
};



