#include "gpio_manager.h"
#include "timer_manager.h"
#include "tmc_manager.h"

extern "C" void SystemClock_Config(void);

using enum TimerId;
using enum GPIOId;

int main(){

    SystemClock_Config();

    TimerManager& timMan = TimerManager::GetInstance();
    timMan.InitTimers();

    GPIOManager& gpioMan = GPIOManager::GetInstance();
    gpioMan.InitGPIOs();

    TMCManager& tmcMan = TMCManager::GetInstance();
    tmcMan.InitTMCs();

    auto tmc = tmcMan.GetTMC(TMCId::TMCX);

    tmc->SetDirection(true);

    if(tmc->Enable()){
        HAL_Delay(2000);
        tmc->SetSpeed(2000);
        HAL_Delay(1000);
        tmc->ToggleDirection();
        HAL_Delay(1000);
        tmc->SetSpeed(4000);
        HAL_Delay(1000);
        tmc->ToggleDirection();
        HAL_Delay(1000);
        tmc->SetDirection(false);
        HAL_Delay(1000);
        tmc->Disable();
    }

    while(1){

    }
}
