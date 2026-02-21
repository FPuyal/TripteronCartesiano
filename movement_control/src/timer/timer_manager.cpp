#include "timer_manager.h"
#include "stm32f4xx_hal.h"

#include <memory>

TimerManager::TimerManager(std::vector<TimerInfo> timersInfos) {
    if (!InitTimers(timersInfos)) {
        // Manejar el error de inicialización, por ejemplo, lanzando una excepción o registrando un error
    }
}

bool TimerManager::InitTimers(std::vector<TimerInfo> timersInfos) {
    // Aquí se inicializan los timers y se agregan a la lista
    for (const auto& timerInfo : timersInfos) {
        if (!SetTimer(timerInfo)) {
            return false;
        }
    }
    return true;
}

bool TimerManager::SetTimer(TimerInfo timerInfo) {
    auto [it, inserted] = mTimersMap.emplace(timerInfo.id, MakeITimer(timerInfo.htim, timerInfo.channel));
    return inserted; // Devuelve true si se insertó correctamente, false si ya existía un Timer con ese ID
}

std::shared_ptr<ITimer> TimerManager::GetTimer(TimerId id) {
    auto it = mTimersMap.find(id);
    if (it != mTimersMap.end()) {
        return it->second;
    }
    return nullptr; // No se encontró el Timer con el ID solicitado
}

std::shared_ptr<ITimerManager> MakeITimerManager(std::vector<TimerInfo> timersInfos){
    return std::make_shared<TimerManager>(timersInfos);
}
