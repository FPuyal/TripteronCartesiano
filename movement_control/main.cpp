#include "tim.h"
#include "gpio.h"

extern "C" void SystemClock_Config(void);
extern "C" void MX_GPIO_Init(void);

int main(){

    SystemClock_Config();
    MX_GPIO_Init();

    MX_TIM2_Init();

    if(HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1) == HAL_OK){
        uint32_t freq = 2000;

        uint32_t timer_clk = HAL_RCC_GetPCLK1Freq();

        // Si el prescaler APB1 > 1, el clock del timer es x2
        if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1)
            timer_clk *= 2;

        uint32_t prescaler = htim2.Init.Prescaler + 1;
        uint32_t arr = (timer_clk / (prescaler * freq)) - 1;

        __HAL_TIM_SET_AUTORELOAD(&htim2, arr);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, arr / 2);  // 50%
    }

    while(1){

    }
}
