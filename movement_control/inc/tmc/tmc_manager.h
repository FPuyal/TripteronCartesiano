#pragma once

#include <map>
#include <memory>
#include <vector>
#include "config.h"
#include "tmc_interface.h"
#include "tmc_manager_interface.h"

class TmcManager :public ITmcManager {
public:
    TmcManager(std::vector<TmcInfo> tmcInfos);
    std::shared_ptr<ITmc> GetTmc(TmcId id);
private:
    bool InitTmcs(std::vector<TmcInfo> tmcInfos);
    bool SetTmc(TmcInfo info);
    std::map<TmcId, std::shared_ptr<ITmc>> mTMCMap;
};
