#include "main.h"
#include "gpio.h"
#include "stm32f4xx_hal_gpio.h"
#include "timer_manager.h"
#include "gpio_wrapper.h"
#include "utils.h"

extern "C" void SystemClock_Config(void);
extern "C" void MX_GPIO_Init(void);

using enum TimerId;
using enum GPIOId;

int main(){

    SystemClock_Config();
    MX_GPIO_Init();

    GPIO gpio(GPIOA, DIR_Pin, GPIOA1);

    TimerManager& timMan = TimerManager::GetInstance();
    timMan.InitTimers();
    gpio.Set();

    if(timMan.GetTimer(GPIOA0)->Start()) {
        timMan.GetTimer(GPIOA0)->SetFrecuency(2000);
        HAL_Delay(1000);
        gpio.Reset();
        HAL_Delay(1000);
        gpio.Set();
        timMan.GetTimer(GPIOA0)->SetFrecuency(4000);
        HAL_Delay(1000);
        timMan.GetTimer(GPIOA0)->Stop();
    }

    while(1){

    }
}
