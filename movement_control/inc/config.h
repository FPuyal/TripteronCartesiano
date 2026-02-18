#pragma once

#include "stm32f407xx.h"
#include <memory>
#include "gpio_wrapper_interface.h"
#include "timer_interface.h"
#include "tmc_interface.h"

enum class GpioId {
    DIR_TMCX,
    EN_TMCX
};

enum class TimerId {
    STEP_TMCX,  // Para TIM2_CH1 en PA0
};

enum class TmcId {
    TMCX,
    TMCY,
    TMCZ
};

struct GpioInfo {
    GpioId id;
    GPIO_TypeDef* gpiox;
    uint16_t pin;
};

struct TimerInfo {
    TimerId id;
    TIM_HandleTypeDef* htim;
    uint32_t channel;  // TIM_CHANNEL_1, TIM_CHANNEL_2, etc.
};

struct TmcInfo {
    TmcId id;
    std::shared_ptr<ITimer> timer;
    std::shared_ptr<IGpioWrapper> dir;
    std::shared_ptr<IGpioWrapper> en;
};
