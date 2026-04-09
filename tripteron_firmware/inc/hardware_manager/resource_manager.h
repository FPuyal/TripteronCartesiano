#pragma once

#include <map>
#include <memory>
#include <vector>

// Clase template que existe unicamente para parametrizar la implementación de todos los
// managers de los recursos que se utilizan.

template <typename IManager, typename IResource, typename ResourceId, typename ResourceInfo>
class ResourceManager : public IManager {

public:
    explicit ResourceManager() = default;

protected:
    bool InitResources(std::vector<ResourceInfo> resInfos) {
        for (const auto& resInfo : resInfos) {
            if (!SetResource(resInfo))
                return false;
        }
        return true;
    }

    std::shared_ptr<IResource> GetResource(ResourceId resId) {
        auto it = mResourcesMap.find(resId);
        if (it != mResourcesMap.end()) {
            return it->second;
        }
        return nullptr; // No se encontró el GPIO con el ID solicitado
    }

    virtual std::shared_ptr<IResource> Make(ResourceInfo resInfo) = 0;

private:
    bool SetResource(ResourceInfo resInfo) {
        auto [it, inserted] = mResourcesMap.emplace(resInfo.id, Make(resInfo));
        return inserted; // Devuelve true si se insertó correctamente, false si ya existía un GPIO con ese ID
    }

    std::map<ResourceId, std::shared_ptr<IResource>> mResourcesMap;
};
