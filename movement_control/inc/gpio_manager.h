#pragma once

#include <cstdint>
#include <locale>
#include <memory>
#include <vector>
#include <set>
#include "gpio_wrapper.h"

class GPIOManager {
public:
    static GPIOManager& GetInstance();
    void InitGPIOs();
    std::shared_ptr<GPIO> GetGPIO(GPIOId id);
private:
    GPIOManager() = default;
    std::vector<std::shared_ptr<GPIO>> mGPIOs;
};



