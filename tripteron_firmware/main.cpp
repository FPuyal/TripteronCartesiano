#include "hardware_manager_interface.h"
#include "step_engine_interface.h"
#include "kinematics_interface.h"
#include "motion_controller_interface.h"

#include "utils.h"

#include <memory>

extern IMotionController* motionControllerInstance;
extern IStepEngine* stepEngineInstance;

int main(){

    auto hardwareManager = MakeIHardwareManager();

    hardwareManager->InitHardware();

    hardwareManager->GetTmc(TmcId::XTmc)->Enable();
    hardwareManager->GetTmc(TmcId::YTmc)->Enable();

    auto stepEngine = MakeIStepEngine(
        19200,
        hardwareManager->GetTmc(TmcId::XTmc),
        hardwareManager->GetTmc(TmcId::YTmc),
        hardwareManager->GetTmc(TmcId::ZTmc)
    );
    stepEngineInstance = stepEngine.get();

    auto motionController = MakeIMotionController(
        MotionConfig{
            {200.0f, 200.0f, 200.0f},
            {240.0f, 240.0f, 48.0f},
            {480.0f, 480.0f, 96.0f},
            0.5f,
            {20.0f, 20.0f, 25.0f}
        }
    );
    motionControllerInstance = motionController.get();

    hardwareManager->GetTimer(TimerId::Tim1)->Start();
    hardwareManager->GetTimer(TimerId::Tim2)->Start();

    bool homingFlagX = false;
    bool homingFlagY = false;
    bool homingFlagZ = false;

    while(!homingFlagX) {
        if(!hardwareManager->GetEndStop(EndStopId::XEnd)->Read()) {
            stepEngine->SetSteps(-500, 0, 0);
        } else {
            stepEngine->SetSteps(0, 0, 0);
            homingFlagX = true;
        }
    }

    while(!homingFlagY) {
        if(!hardwareManager->GetEndStop(EndStopId::YEnd)->Read()) {
            stepEngine->SetSteps(0, -500, 0);
        } else {
            stepEngine->SetSteps(0, 0, 0);
            homingFlagY = true;
        }
    }

    while(!homingFlagZ) {
        if(!hardwareManager->GetEndStop(EndStopId::ZEnd)->Read()) {
            stepEngine->SetSteps(0, 0, -500);
        } else {
            stepEngine->SetSteps(0, 0, 0);
            homingFlagZ = true;
        }
    }

    motionController->SetHomePosition();


    MotionData segments[] = {
        // Punto de partida del círculo (base de la espiral)
        {175.00f, 100.00f, 40.00f},

        // --- Subida (1 vuelta completa, Z: 40 -> 160, paso de 7.50°) ---
        {174.36f, 109.79f, 42.50f},
        {172.44f, 119.41f, 45.00f},
        {169.29f, 128.70f, 47.50f},
        {164.95f, 137.50f, 50.00f},
        {159.50f, 145.66f, 52.50f},
        {153.03f, 153.03f, 55.00f},
        {145.66f, 159.50f, 57.50f},
        {137.50f, 164.95f, 60.00f},
        {128.70f, 169.29f, 62.50f},
        {119.41f, 172.44f, 65.00f},
        {109.79f, 174.36f, 67.50f},
        {100.00f, 175.00f, 70.00f},
        {90.21f,  174.36f, 72.50f},
        {80.59f,  172.44f, 75.00f},
        {71.30f,  169.29f, 77.50f},
        {62.50f,  164.95f, 80.00f},
        {54.34f,  159.50f, 82.50f},
        {46.97f,  153.03f, 85.00f},
        {40.50f,  145.66f, 87.50f},
        {35.05f,  137.50f, 90.00f},
        {30.71f,  128.70f, 92.50f},
        {27.56f,  119.41f, 95.00f},
        {25.64f,  109.79f, 97.50f},
        {25.00f,  100.00f, 100.00f},
        {25.64f,  90.21f,  102.50f},
        {27.56f,  80.59f,  105.00f},
        {30.71f,  71.30f,  107.50f},
        {35.05f,  62.50f,  110.00f},
        {40.50f,  54.34f,  112.50f},
        {46.97f,  46.97f,  115.00f},
        {54.34f,  40.50f,  117.50f},
        {62.50f,  35.05f,  120.00f},
        {71.30f,  30.71f,  122.50f},
        {80.59f,  27.56f,  125.00f},
        {90.21f,  25.64f,  127.50f},
        {100.00f, 25.00f,  130.00f},
        {109.79f, 25.64f,  132.50f},
        {119.41f, 27.56f,  135.00f},
        {128.70f, 30.71f,  137.50f},
        {137.50f, 35.05f,  140.00f},
        {145.66f, 40.50f,  142.50f},
        {153.03f, 46.97f,  145.00f},
        {159.50f, 54.34f,  147.50f},
        {164.95f, 62.50f,  150.00f},
        {169.29f, 71.30f,  152.50f},
        {172.44f, 80.59f,  155.00f},
        {174.36f, 90.21f,  157.50f},
        {175.00f, 100.00f, 160.00f}, // arriba del todo

        // --- Bajada (1 vuelta completa, Z: 160 -> 40, paso de 7.50°) ---
        {174.36f, 109.79f, 157.50f},
        {172.44f, 119.41f, 155.00f},
        {169.29f, 128.70f, 152.50f},
        {164.95f, 137.50f, 150.00f},
        {159.50f, 145.66f, 147.50f},
        {153.03f, 153.03f, 145.00f},
        {145.66f, 159.50f, 142.50f},
        {137.50f, 164.95f, 140.00f},
        {128.70f, 169.29f, 137.50f},
        {119.41f, 172.44f, 135.00f},
        {109.79f, 174.36f, 132.50f},
        {100.00f, 175.00f, 130.00f},
        {90.21f,  174.36f, 127.50f},
        {80.59f,  172.44f, 125.00f},
        {71.30f,  169.29f, 122.50f},
        {62.50f,  164.95f, 120.00f},
        {54.34f,  159.50f, 117.50f},
        {46.97f,  153.03f, 115.00f},
        {40.50f,  145.66f, 112.50f},
        {35.05f,  137.50f, 110.00f},
        {30.71f,  128.70f, 107.50f},
        {27.56f,  119.41f, 105.00f},
        {25.64f,  109.79f, 102.50f},
        {25.00f,  100.00f, 100.00f},
        {25.64f,  90.21f,  97.50f},
        {27.56f,  80.59f,  95.00f},
        {30.71f,  71.30f,  92.50f},
        {35.05f,  62.50f,  90.00f},
        {40.50f,  54.34f,  87.50f},
        {46.97f,  46.97f,  85.00f},
        {54.34f,  40.50f,  82.50f},
        {62.50f,  35.05f,  80.00f},
        {71.30f,  30.71f,  77.50f},
        {80.59f,  27.56f,  75.00f},
        {90.21f,  25.64f,  72.50f},
        {100.00f, 25.00f,  70.00f},
        {109.79f, 25.64f,  67.50f},
        {119.41f, 27.56f,  65.00f},
        {128.70f, 30.71f,  62.50f},
        {137.50f, 35.05f,  60.00f},
        {145.66f, 40.50f,  57.50f},
        {153.03f, 46.97f,  55.00f},
        {159.50f, 54.34f,  52.50f},
        {164.95f, 62.50f,  50.00f},
        {169.29f, 71.30f,  47.50f},
        {172.44f, 80.59f,  45.00f},
        {174.36f, 90.21f,  42.50f},
        {175.00f, 100.00f, 40.00f}   // cierre: vuelve exactamente al punto de partida
    };

    /* MotionData segments[] = {
        {20.0f, 20.0f, 20.0f},
        {150.0f, 20.0f, 20.0f}
    }; */

    motionController->SetSegments(segments, sizeof(segments) / sizeof(segments[0]));

    while(1){
        motionController->Move();
        motionController->Update();
        stepEngine->SetSteps(motionController->GetSteps().x, motionController->GetSteps().y, motionController->GetSteps().z);
    }

}
