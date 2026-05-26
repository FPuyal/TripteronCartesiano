#pragma once

#include "i2c_manager_interface.h"
#include "resource_manager.h"

#include <map>

class I2CManager : public ResourceManager<II2CManager, II2CWrapper, I2cId, I2cInfo> {
public:
    I2CManager(std::vector<I2cInfo> i2csInfo) : ResourceManager() {
        InitResources(i2csInfo);
    }
    std::shared_ptr<II2CWrapper> GetI2C(I2cId id) override {
        return GetResource(id);
    }
protected:
    std::shared_ptr<II2CWrapper> Make(I2cInfo i2cInfo) override {
        return MakeII2CWrapper(i2cInfo.hi2c, i2cInfo.devAddress);
    }
};
