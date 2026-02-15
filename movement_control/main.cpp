#include "gpio_manager.h"
#include "gpio_wrapper.h"
#include "timer_manager.h"
#include "tmc.h"
#include "utils.h"

extern "C" void SystemClock_Config(void);

using enum TimerId;
using enum GPIOId;

int main(){

    SystemClock_Config();

    TimerManager& timMan = TimerManager::GetInstance();
    timMan.InitTimers();

    GPIOManager& gpioMan = GPIOManager::GetInstance();
    gpioMan.InitGPIOs();

    TMC tmc(timMan.GetTimer(GPIOA0), gpioMan.GetGPIO(GPIOA1), gpioMan.GetGPIO(GPIOA2));

    tmc.SetDirection(true);

    if(tmc.Enable()){
        HAL_Delay(10000);
        tmc.SetSpeed(2000);
        HAL_Delay(1000);
        tmc.ToggleDirection();
        HAL_Delay(1000);
        tmc.SetSpeed(4000);
        HAL_Delay(1000);
        tmc.ToggleDirection();
        HAL_Delay(1000);
        tmc.SetDirection(false);
        HAL_Delay(1000);
        tmc.Disable();
    }

    while(1){

    }
}
