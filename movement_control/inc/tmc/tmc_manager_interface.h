#pragma once

#include "tmc_interface.h"
#include "utils.h"

#include <memory>
#include <vector>

class ITmcManager {
public:
    virtual ~ITmcManager() = default;
    virtual std::shared_ptr<ITmc> GetTmc(TmcId id) = 0;
};

std::shared_ptr<ITmcManager> MakeITmcManager(std::vector<TmcInfo> tmcInfos);
