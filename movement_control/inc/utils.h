#pragma once

enum class TimerId {
    GPIOA0,  // Para TIM2_CH1 en PA0
    GPIOB5,  // Para TIM3_CH2 en PB5
    GPIOC6  // Para TIM4_CH1 en PC6
};

enum class GPIOId {
    GPIOA1,
    GPIOA2
};

enum class TMCId {
    TMCX,
    TMCY,
    TMCZ
};

struct TMCConfig {
    TimerId stepId;
    GPIOId dirId;
    GPIOId enId;
};
