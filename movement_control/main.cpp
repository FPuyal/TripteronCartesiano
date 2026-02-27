#include "gpio.h"
#include "i2c_wrapper_interface.h"
#include "stm32f4xx_hal.h"

#include "i2c.h"

#include "i2c_manager_interface.h"
#include <memory>

extern "C" void SystemClock_Config(void);

#define AS5600_ADDR (0x36 << 1)
#define AS5600_RAW_ANGLE 0x0C
#define AS5600_ANGLE 0x0E
#define ZPOS_H 0x01

int main(){

    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C2_Init();   // <<< Inicializa I2C2

    std::shared_ptr<II2CManager> i2cManager = MakeII2CManager({{I2cId::I2CX,&hi2c2, AS5600_ADDR}});

    std::shared_ptr<II2CWrapper> i2cX = i2cManager->GetI2C(I2cId::I2CX);

    uint16_t init_raw_angle = 0;
    uint16_t raw_angle = 0;
    double angle;

    i2cX->SetMemAddress(AS5600_RAW_ANGLE);
    i2cX->Read(init_raw_angle);
    i2cX->SetMemAddress(ZPOS_H);
    bool escritura = i2cX->Write(init_raw_angle);  

    i2cX->SetMemAddress(AS5600_ANGLE);

    HAL_Delay(1);  // prueba más rápida (~1 kHz)

    while (1) {

        i2cX->Read(raw_angle);

        HAL_Delay(1);  // prueba más rápida (~1 kHz)

        angle = 360 - ((double)(raw_angle)) * 360 / 4096;

    }

}
