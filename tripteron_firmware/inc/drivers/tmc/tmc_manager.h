#pragma once

#include "tmc_manager_interface.h"
#include "resource_manager.h"

class TmcManager : public ResourceManager<ITmcManager, ITmc, TmcId, TmcInfo> {
public:
    TmcManager(std::vector<TmcInfo> tmcsInfo) : ResourceManager() {
        InitResources(tmcsInfo);
    }
    std::shared_ptr<ITmc> GetTmc(TmcId id) override {
        return GetResource(id);
    }
protected:
    std::shared_ptr<ITmc> Make(TmcInfo tmcInfo) override {
        return MakeITmc(tmcInfo.timer, tmcInfo.dir, tmcInfo.en, tmcInfo.uart, tmcInfo.nodeAddress, tmcInfo.microSteps);
    }
};
