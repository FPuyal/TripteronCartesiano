#pragma once

#include <memory>
#include <vector>
#include "tmc_interface.h"
#include "config.h"

class ITmcManager {
public:
    virtual ~ITmcManager() = default;
    virtual std::shared_ptr<ITmc> GetTmc(TmcId id) = 0;
};

std::shared_ptr<ITmcManager> MakeITmcManager(std::vector<TmcInfo> tmcInfos);
