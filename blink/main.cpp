#include "gpio.h"

extern "C" void SystemClock_Config(void);

int main(){

    SystemClock_Config();
    MX_GPIO_Init();

    while(1){
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
        HAL_Delay(1000);
    }
}
