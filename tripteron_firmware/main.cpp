#include "hardware_manager_interface.h"
#include "step_engine_interface.h"
#include "kinematics_interface.h"
#include "robot_interface.h"
#include "comms_interface.h"

#include "stm32f4xx_hal.h"
#include "usb_cdc_interface.h"

#include <memory>

extern IStepEngine* stepEngineInstance;
extern IRobot* robotInterface;
extern IKinematics* kinematicsInstance;

/*
// TEST DE ENCODER

#include "i2c.h"   // hi2c1, hi2c2, hi2c3

struct EncoderDiag {
    bool md, ml, mh;      // imán detectado / débil / fuerte
    uint8_t agc;
    uint16_t magnitude;
    uint16_t rawAngle;
    bool ok;              // false si falló alguna lectura I2C
};

EncoderDiag encoderDiag[3];   // global: visible en Watch / Live Watch

static EncoderDiag ReadEncoderDiag(I2C_HandleTypeDef* hi2c) {
    constexpr uint16_t kAddr = 0x36 << 1;
    EncoderDiag d{};
    uint8_t buf[2];

    d.ok = HAL_I2C_Mem_Read(hi2c, kAddr, 0x0B, I2C_MEMADD_SIZE_8BIT, buf, 1, 10) == HAL_OK;
    d.md = buf[0] & (1 << 5);
    d.ml = buf[0] & (1 << 4);
    d.mh = buf[0] & (1 << 3);

    d.ok &= HAL_I2C_Mem_Read(hi2c, kAddr, 0x1A, I2C_MEMADD_SIZE_8BIT, buf, 1, 10) == HAL_OK;
    d.agc = buf[0];

    d.ok &= HAL_I2C_Mem_Read(hi2c, kAddr, 0x1B, I2C_MEMADD_SIZE_8BIT, buf, 2, 10) == HAL_OK;
    d.magnitude = ((buf[0] << 8) | buf[1]) & 0x0FFF;

    d.ok &= HAL_I2C_Mem_Read(hi2c, kAddr, 0x0C, I2C_MEMADD_SIZE_8BIT, buf, 2, 10) == HAL_OK;
    d.rawAngle = ((buf[0] << 8) | buf[1]) & 0x0FFF;

    return d;
}
*/

int main(){

   auto hardwareManager = MakeIHardwareManager();

    if(!hardwareManager->InitHardware())
        while(1) {}

/*
    encoderDiag[0] = ReadEncoderDiag(&hi2c1);   // X
    encoderDiag[1] = ReadEncoderDiag(&hi2c2);   // Y
    encoderDiag[2] = ReadEncoderDiag(&hi2c3);   // Z
*/

   auto comms = MakeIComms(hardwareManager->GetUsbCdc());

    auto stepEngine = MakeIStepEngine(
        hardwareManager->GetTmc(TmcId::XTmc),
        hardwareManager->GetTmc(TmcId::YTmc),
        hardwareManager->GetTmc(TmcId::ZTmc),
        19200
    );
    stepEngineInstance = stepEngine.get();

    auto kinematics = MakeIKinematics(
        hardwareManager->GetEncoder(EncoderId::XEncoder),
        hardwareManager->GetEncoder(EncoderId::YEncoder),
        hardwareManager->GetEncoder(EncoderId::ZEncoder)
    );
    kinematicsInstance = kinematics.get();

    auto robot = MakeIRobot(stepEngine,
        hardwareManager->GetEndStop(EndStopId::XEnd),
        hardwareManager->GetEndStop(EndStopId::YEnd),
        hardwareManager->GetEndStop(EndStopId::ZEnd),
        kinematics,
        comms
    );
    robotInterface = robot.get();

    while(1){
        robot->Run();
    }
}
