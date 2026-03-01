#include "encoder_manager.h"

#include <memory>

EncoderManager::EncoderManager(std::vector<EncoderInfo> encodersInfos) {
    if (!InitEncoders(encodersInfos)) {
        // Manejar el error de inicialización si es necesario
    }
}

bool EncoderManager::InitEncoders(std::vector<EncoderInfo> encodersInfos) {
    for (const auto& encoderInfo : encodersInfos) {
        if (!SetEncoder(encoderInfo)) {
            return false;
        }
    }
    return true;
}

bool EncoderManager::SetEncoder(EncoderInfo encoderInfo) {
    auto [it, inserted] = mEncodersMap.emplace(encoderInfo.id, MakeIEncoder(encoderInfo.i2c));
    return inserted;
}

std::shared_ptr<IEncoder> EncoderManager::GetEncoder(EncoderId id) {
    auto it = mEncodersMap.find(id);
    if (it != mEncodersMap.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<IEncoderManager> MakeIEncoderManager(std::vector<EncoderInfo> encodersInfos){
    return std::make_shared<EncoderManager>(encodersInfos);
}
