#pragma once

#include "encoder_manager_interface.h"

#include <map>

class EncoderManager : public IEncoderManager {
public:
    EncoderManager(std::vector<EncoderInfo> encoderInfos);
    std::shared_ptr<IEncoder> GetEncoder(EncoderId id) override;
private:
    bool InitEncoders(std::vector<EncoderInfo> encoderInfos);
    bool SetEncoder(EncoderInfo encoderInfo);
    std::map<EncoderId, std::shared_ptr<IEncoder>> mEncodersMap;
};

