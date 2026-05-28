#pragma once

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#include <memory>

enum class EndStopId {
    XEnd = 0,
    YEnd,
    ZEnd,
    COUNT
};

enum class TmcId {
    XTmc = 0,
    YTmc,
    ZTmc,
    COUNT
};

enum class EncoderId {
    XEncoder = 0,
    YEncoder,
    ZEncoder,
    COUNT
};

struct AxisConfig {
    double stepsMmRatio;
    double maxPosition;
    double maxVelocity;
    double maxAcceleration;
};
