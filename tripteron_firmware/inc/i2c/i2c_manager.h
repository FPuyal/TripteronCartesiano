#pragma once

#include "i2c_manager_interface.h"
#include "i2c_wrapper_interface.h"

#include <map>

class I2CManager : public II2CManager {
public:
    I2CManager(std::vector<I2CInfo> i2cInfos);
    std::shared_ptr<II2CWrapper> GetI2C(I2cId id) override;
private:
    bool InitI2Cs(std::vector<I2CInfo> i2cInfos);
    bool SetI2C(I2CInfo i2cInfo);
    std::map<I2cId, std::shared_ptr<II2CWrapper>> mI2CsMap;
};
