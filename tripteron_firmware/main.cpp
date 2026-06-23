#include "step_engine_interface.h"
#include "hardware_manager_interface.h"
#include "utils.h"

int main(){

    auto hardwareManager = MakeIHardwareManager();

    hardwareManager->InitHardware();

    extern IStepEngine* stepEngineInstance;

    auto stepEngine = MakeIStepEngine(
        19200,
        *hardwareManager->GetTmc(TmcId::XTmc)
    );
    stepEngineInstance = stepEngine.get();

    hardwareManager->GetTimer(TimerId::Timer1)->Start();

    int16_t steps[3] = {0, 0, 0};
    const int16_t target = 9600;
    const int16_t increment = 100;   // cuánto sube cada escalón
    const uint32_t rampDelayMs = 10; // tiempo entre escalones

    while(1){
        // Rampa de subida hasta target
        if(steps[0] < target){
            steps[0] += increment;
            if(steps[0] > target) steps[0] = target;
            stepEngine->SetSteps(steps);
            HAL_Delay(rampDelayMs);
        }
        stepEngine->Update();
    }
}



