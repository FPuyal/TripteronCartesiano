#pragma once

#include "encoder_interface.h"
#include "utils.h"

#include <vector>

class IEncoderManager {
public:
    virtual ~IEncoderManager() = default;
    virtual std::shared_ptr<IEncoder> GetEncoder(EncoderId id) = 0;
};

std::shared_ptr<IEncoderManager> MakeIEncoderManager(std::vector<EncoderInfo> encoderInfos);
