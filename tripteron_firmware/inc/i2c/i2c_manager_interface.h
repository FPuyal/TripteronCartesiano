#pragma once

#include "i2c_wrapper_interface.h"
#include "utils.h"

#include <memory>
#include <vector>

class II2CManager {
public:
    virtual ~II2CManager() = default;
    virtual std::shared_ptr<II2CWrapper> GetI2C(I2cId id) = 0;
};

std::shared_ptr<II2CManager> MakeII2CManager(std::vector<I2cInfo> i2cInfos);

