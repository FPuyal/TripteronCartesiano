#include "i2c_manager.h"

I2CManager::I2CManager(std::vector<I2CInfo> I2cInfos) {
    if (!InitI2Cs(I2cInfos)) {
        // Manejar el error de inicialización
    }
}

bool I2CManager::InitI2Cs(std::vector<I2CInfo> I2cInfos) {
    // Aquí se inicializan los I2cs y se agregan a la lista
    for (const auto& I2cInfo : I2cInfos) {
        if (!SetI2C(I2cInfo)) {
            return false; // Si alguno falla, se devuelve false
        }
    }
    return true;
}

bool I2CManager::SetI2C(I2CInfo I2cInfo) {
    auto [it, inserted] = mI2CsMap.emplace(I2cInfo.id, MakeII2CWrapper(I2cInfo.hi2c, I2cInfo.devAddress));
    return inserted; // Devuelve true si se insertó correctamente, false si ya existía un I2C con ese ID
}

std::shared_ptr<II2CWrapper> I2CManager::GetI2C(I2cId id) {
    auto it = mI2CsMap.find(id);
    if (it != mI2CsMap.end()) {
        return it->second;
    }
    return nullptr; // No se encontró el I2C con el ID solicitado
}

std::shared_ptr<II2CManager> MakeII2CManager(std::vector<I2CInfo> I2cInfos) {
    return std::make_shared<I2CManager>(I2cInfos);
}

