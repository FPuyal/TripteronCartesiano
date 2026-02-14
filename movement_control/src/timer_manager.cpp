#include "timer_manager.h"
#include "main.h"
#include "tim.h" // Tiene que ser exclusivo de este .cpp
#include "utils.h"
#include <cassert>

using enum TimerId;

TimerManager& TimerManager::GetInstance(){
    static TimerManager instance;
    return instance;
}

bool TimerManager::InitTimers() {
    // Aquí se inicializan los timers y se agregan a la lista
    MX_TIM2_Init();
    return AddTimer(GPIOA0, &htim2, TIM_CHANNEL_1);
}

bool TimerManager::AddTimer(TimerId id, TIM_HandleTypeDef* htim, uint16_t channel) {
    auto [it, inserted] = mTimersMap.emplace(id, std::make_shared<Timer>(htim, channel));
    return inserted; // Devuelve true si se insertó correctamente, false si ya existía un Timer con ese ID
}


std::shared_ptr<Timer> TimerManager::GetTimer(TimerId id) {
    auto it = mTimersMap.find(id);
    if (it != mTimersMap.end()) {
        return it->second;
    }
    return nullptr; // No se encontró el Timer con el ID solicitado
}
