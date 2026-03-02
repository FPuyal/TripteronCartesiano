#pragma once

#include "encoder_manager_interface.h"
#include "resource_manager.h"

#include <map>

class EncoderManager : public ResourceManager<IEncoderManager, IEncoder, EncoderId, EncoderInfo> {
public:
    EncoderManager(std::vector<EncoderInfo> encodersInfo) : ResourceManager() {
        InitResources(encodersInfo);
    }
    std::shared_ptr<IEncoder> GetEncoder(EncoderId id) override {
        return GetResource(id);
    }
protected:
    std::shared_ptr<IEncoder> Make(EncoderInfo encoderInfo) override {
        return MakeIEncoder(encoderInfo.i2c);
    }
};

