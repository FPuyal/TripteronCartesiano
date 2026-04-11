#include "gpio_manager_interface.h"
#include "gpio_manager.h"

#include "timer_manager_interface.h"
#include "timer_manager.h"

#include "i2c_manager_interface.h"
#include "i2c_manager.h"

#include "tmc_manager_interface.h"
#include "tmc_manager.h"

#include "encoder_manager_interface.h"
#include "encoder_manager.h"

std::shared_ptr<IGpioManager> MakeIGpioManager(std::vector<GpioInfo> gpiosInfo){
    return std::make_shared<GpioManager>(gpiosInfo);
}

std::shared_ptr<IEncoderManager> MakeIEncoderManager(std::vector<EncoderInfo> encodersInfo){
    return std::make_shared<EncoderManager>(encodersInfo);
}

std::shared_ptr<II2CManager> MakeII2CManager(std::vector<I2cInfo> i2csInfo){
    return std::make_shared<I2CManager>(i2csInfo);
}

std::shared_ptr<ITimerManager> MakeITimerManager(std::vector<TimerInfo> timersInfo){
    return std::make_shared<TimerManager>(timersInfo);
}

std::shared_ptr<ITmcManager> MakeITmcManager(std::vector<TmcInfo> tmcsInfo){
    return std::make_shared<TmcManager>(tmcsInfo);
}

