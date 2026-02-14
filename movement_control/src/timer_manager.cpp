#include "timer_manager.h"
#include "tim.h" // Tiene que ser exclusivo de este .cpp

using enum TimerId;

TimerManager& TimerManager::GetInstance(){
    static TimerManager instance;
    return instance;
}

void TimerManager::InitTimers() {
    // Aquí se inicializan los timers y se agregan a la lista
    MX_TIM2_Init();
    mTimers.push_back(std::make_shared<Timer>(&htim2, TIM_CHANNEL_1, GPIOA0));
}

std::shared_ptr<Timer> TimerManager::GetTimer(TimerId id) {
    for (const auto& timer : mTimers) {
        if (timer->mId == id) return timer;
    }
    return nullptr; // No se encontró el Timer con el ID solicitado
}
