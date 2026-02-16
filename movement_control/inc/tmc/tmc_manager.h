#pragma once

#include "tmc.h"
#include "utils.h"
#include <map>
#include <memory>
#include <variant>
#include <vector>

class TMCManager {
public:
    static TMCManager& GetInstance();
    bool InitTMCs();
    std::shared_ptr<TMC> GetTMC(TMCId id);
private:
    TMCManager() = default;
    bool AddTMC(TMCId id, std::shared_ptr<Timer> step, std::shared_ptr<GPIO> dir, std::shared_ptr<GPIO> en);
    void SetupTMCConfig();
    std::map<TMCId, std::shared_ptr<TMC>> mTMCMap;
    std::map<TMCId, TMCConfig> mTMCConfigMap;
};
