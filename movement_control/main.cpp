#include "gpio.h"
#include "hardware_manager_interface.h"
#include "stm32f4xx_hal.h"

#include "i2c.h"

extern "C" void SystemClock_Config(void);

#define AS5600_ADDR (0x36 << 1)
#define AS5600_ANGLE_REG 0x0E

int main(){

    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C2_Init();   // <<< Inicializa I2C2

    uint16_t angle_raw = 0;
    uint16_t init_angle_raw = 0;
    int angle;
    uint8_t buffer[2];

    while (1) {
        if (HAL_I2C_Mem_Read(&hi2c2,
                         AS5600_ADDR,
                         AS5600_ANGLE_REG,
                         I2C_MEMADD_SIZE_8BIT,
                         buffer,
                         2,
                         10) == HAL_OK)
        {
            init_angle_raw = ((uint16_t)buffer[0] << 8) | buffer[1];
            init_angle_raw &= 0x0FFF;
        }

        HAL_Delay(1);  // prueba más rápida (~1 kHz)

        if (HAL_I2C_Mem_Read(&hi2c2,
                         AS5600_ADDR,
                         AS5600_ANGLE_REG,
                         I2C_MEMADD_SIZE_8BIT,
                         buffer,
                         2,
                         10) == HAL_OK)
        {
            angle_raw = ((uint16_t)buffer[0] << 8) | buffer[1];
            angle_raw &= 0x0FFF;
        }

        HAL_Delay(1);  // prueba más rápida (~1 kHz)

        angle = ((int)(angle_raw) - (int)(init_angle_raw))*360 / 4096;

    }

}
