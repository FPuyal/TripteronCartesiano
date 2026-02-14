#include "gpio.h"
#include "timer_manager.h"

extern "C" void SystemClock_Config(void);
extern "C" void MX_GPIO_Init(void);

using enum TimerId;

int main(){

    SystemClock_Config();
    MX_GPIO_Init();

    TimerManager& timMan = TimerManager::GetInstance();
    timMan.InitTimers();

    if(timMan.GetTimer(GPIOA0)->Start()) {
        timMan.GetTimer(GPIOA0)->SetFrecuency(2000);
        HAL_Delay(1000);
        timMan.GetTimer(GPIOA0)->SetFrecuency(100);
        HAL_Delay(1000);
        timMan.GetTimer(GPIOA0)->SetFrecuency(4000);
        HAL_Delay(1000);
        timMan.GetTimer(GPIOA0)->Stop();
    }

    while(1){

    }
}
