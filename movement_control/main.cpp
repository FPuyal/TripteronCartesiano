#include "hardware_manager_interface.h"
#include "stm32f4xx_hal.h"

int main(){

    auto hardwareManager = MakeIHardwareManager();
    hardwareManager->InitHardware();

    auto tmcs = hardwareManager->GetTmcs();

    tmcs[TmcId::TMCX]->Enable();
    tmcs[TmcId::TMCX]->SetDirection(true);

    tmcs[TmcId::TMCX]->SetSpeed(1000);
    HAL_Delay(1000);
    tmcs[TmcId::TMCX]->SetSpeed(5000);
    HAL_Delay(1000);
    tmcs[TmcId::TMCX]->ToggleDirection();
    tmcs[TmcId::TMCX]->SetSpeed(1000);
    HAL_Delay(1000);
    tmcs[TmcId::TMCX]->Disable();

    while(1){

    }
}
