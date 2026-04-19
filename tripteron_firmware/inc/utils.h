#pragma once

#include "stm32f407xx.h"
#include "stm32f4xx_hal.h"

#include <memory>

class IGpioOutput;
class ITimer;
class ITmc;
class II2CWrapper;

enum class GpioId {
    DIR_TMCX,
    EN_TMCX,
    DIR_TMCY,
    EN_TMCY,
    DIR_TMCZ,
    EN_TMCZ,
    END_STOP_X,
    END_STOP_Y,
    END_STOP_Z
};

enum class GpioMode {
    INPUT,
    OUTPUT
};

struct GpioInfo {
    GpioId id; // Debe ser siempre el primer elemento
    GpioMode mode;
    GPIO_TypeDef* gpiox;
    uint16_t pin;
};

enum class TimerId {
    STEP_TMCX,  // Para TIM2_CH1 en PA0
    STEP_TMCY,  // Para TIM2_CH2 en PA1
    STEP_TMCZ   // Para TIM2_CH3 en PA2
};


struct TimerInfo {
    TimerId id;
    TIM_HandleTypeDef* htim;
    uint32_t channel;  // TIM_CHANNEL_1, TIM_CHANNEL_2, etc.
};


enum class TmcId {
    TMCX = 0,
    TMCY,
    TMCZ,
    COUNT
};

struct TmcInfo {
    TmcId id;
    std::shared_ptr<ITimer> timer;
    std::shared_ptr<IGpioOutput> en;
    std::shared_ptr<IGpioOutput> dir;
    std::shared_ptr<IUart> uart;
    uint16_t microSteps;
    uint8_t nodeAddr;
};

enum class I2cId {
    I2CX,
    I2CY,
    I2CZ
};

struct I2cInfo {
    I2cId id;
    I2C_HandleTypeDef* hi2c;
    uint16_t devAddress;
};

enum class EncoderId {
    EncoderX = 0,
    EncoderY,
    EncoderZ,
    COUNT
};

struct EncoderInfo {
    EncoderId id;
    std::shared_ptr<II2CWrapper> i2c;
};
