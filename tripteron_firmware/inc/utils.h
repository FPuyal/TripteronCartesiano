#pragma once

#include <memory>

#include "gpio_wrapper_interface.h"
#include "timer_interface.h"
#include "tmc_interface.h"
#include "i2c_wrapper_interface.h"

enum class GpioId {
    DIR_TMCX,
    EN_TMCX
};

enum class TimerId {
    STEP_TMCX  // Para TIM2_CH1 en PA0
};

enum class TmcId {
    TMCX = 0,
    TMCY,
    TMCZ,
    COUNT
};

enum class I2cId {
    I2CX,
    I2CY,
    I2CZ
};

enum class EncoderId {
    EncoderX = 0,
    EncoderY,
    EncoderZ,
    COUNT
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
    std::shared_ptr<IGpioWrapper> en;
    std::shared_ptr<IGpioWrapper> dir;
};

struct I2CInfo {
    I2cId id;
    I2C_HandleTypeDef* hi2c;
    uint16_t devAddress;
};

struct EncoderInfo {
    EncoderId id;
    std::shared_ptr<II2CWrapper> i2c;
};
