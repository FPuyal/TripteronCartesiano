#include "gpio_manager.h"
#include "gpio_wrapper.h"
#include "timer_manager.h"
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

    gpioMan.GetGPIO(GPIOA1)->Set();

    if(timMan.GetTimer(GPIOA0)->Start()) {
        timMan.GetTimer(GPIOA0)->SetFrecuency(2000);
        HAL_Delay(1000);
        gpioMan.GetGPIO(GPIOA1)->Toggle();
        HAL_Delay(1000);
        timMan.GetTimer(GPIOA0)->SetFrecuency(4000);
        HAL_Delay(1000);
        gpioMan.GetGPIO(GPIOA1)->Toggle();
        HAL_Delay(1000);
        gpioMan.GetGPIO(GPIOA1)->Reset();
        HAL_Delay(1000);
        timMan.GetTimer(GPIOA0)->Stop();
    }

    while(1){

    }
}
