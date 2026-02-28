#include "encoder_interface.h"
#include "gpio.h"
#include "stm32f4xx_hal.h"

#include "i2c.h"

#include "i2c_manager_interface.h"
#include <memory>
#include <cmath>

extern "C" void SystemClock_Config(void);

#define AS5600_ADDR (0x36 << 1)

int main(){

    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C2_Init();   // <<< Inicializa I2C2

    std::shared_ptr<II2CManager> i2cManager = MakeII2CManager({{I2cId::I2CX,&hi2c2, AS5600_ADDR}});

    std::shared_ptr<IEncoder> encX = MakeIEncoder(i2cManager->GetI2C(I2cId::I2CX));

    uint16_t raw_angle = 0;
    double angle;
    double angle_aux;
    bool lectura;

    encX->SetOffset();

    while (1) {

        lectura = encX->ReadAngle(raw_angle);

        HAL_Delay(1);  // prueba más rápida (~1 kHz)

        angle_aux = 360 - ((double)(raw_angle)) * 360.0 / 4096.0;
        angle = angle_aux == 360 ? 0 : angle_aux;

    }

}
