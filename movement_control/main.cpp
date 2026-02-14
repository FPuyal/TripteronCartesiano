#include "gpio.h"
#include "timer.h"
#include "tim.h"

extern "C" void SystemClock_Config(void);
extern "C" void MX_GPIO_Init(void);

int main(){

    SystemClock_Config();
    MX_GPIO_Init();

    MX_TIM2_Init();

    using enum TimerId;

    Timer tim(&htim2, TIM_CHANNEL_1, TimerId::GPIOA0);

    if(tim.Start()) {
        tim.SetFrecuency(2000);
        HAL_Delay(1000);
        tim.SetFrecuency(3000);
        HAL_Delay(1000);
        tim.SetFrecuency(300);
        HAL_Delay(1000);
        tim.Stop();
    }

    while(1){

    }
}
