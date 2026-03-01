#include "encoder_manager_interface.h"
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

    std::shared_ptr<II2CManager> i2cMan = MakeII2CManager({{I2cId::I2CX,&hi2c2, AS5600_ADDR}});

    std::shared_ptr<IEncoderManager> encMan = MakeIEncoderManager({{EncoderId::EncoderX, i2cMan->GetI2C(I2cId::I2CX)}});

    std::shared_ptr<IEncoder> encX = encMan->GetEncoder(EncoderId::EncoderX);

    double angle;
    bool lectura;

    encX->SetOffset();

    while (1) {

        lectura = encX->ReadAngle(angle);

        HAL_Delay(1);  // prueba más rápida (~1 kHz)
    }

}
