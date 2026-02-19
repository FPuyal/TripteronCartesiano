#pragma once

#include "tmc_interface.h"
#include "utils.h"

#include <memory>
#include <map>

class IHardwareManager {
public:
    virtual ~IHardwareManager() = default;
    virtual void InitHardware() = 0;
    virtual std::map<TmcId, std::shared_ptr<ITmc>> GetTmcs() = 0;
};

std::shared_ptr<IHardwareManager> MakeIHardwareManager();
