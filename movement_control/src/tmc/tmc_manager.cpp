#include "tmc_manager.h"
#include "config.h"
#include "tmc_interface.h"
#include <memory>

TmcManager::TmcManager(std::vector<TmcInfo> tmcInfos) {
    if (!InitTmcs(tmcInfos)) {
        // Manejar error de inicialización
    }
}

bool TmcManager::InitTmcs(std::vector<TmcInfo> tmcInfos) {
    // Aquí se inicializan los TMCs y se agregan a la lista
    for (const auto& info : tmcInfos) {
        if (!SetTmc(info)) {
            return false; // Error al configurar un TMC
        }
    }
    return true;
}

bool TmcManager::SetTmc(TmcInfo info) {
    auto [it, inserted] = mTMCMap.emplace(info.id, MakeITmc(info.timer, info.dir, info.en));
    return inserted; // Devuelve true si se insertó correctamente, false si ya existía un TMC con ese ID
}

std::shared_ptr<ITmc> TmcManager::GetTmc(TmcId id) {
    auto it = mTMCMap.find(id);
    if (it != mTMCMap.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<ITmcManager> MakeITmcManager(std::vector<TmcInfo> tmcInfos) {
    return std::make_shared<TmcManager>(tmcInfos);
}

