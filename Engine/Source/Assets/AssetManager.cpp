#include "SpoonEngine/Assets/AssetManager.h"

namespace SpoonEngine {
    
    void AssetManager::UnloadAsset(AssetID id) {
        auto it = m_Assets.find(id);
        if (it != m_Assets.end()) {
            auto asset = it->second;
            m_PathToAssetMap.erase(asset->GetPath());
            m_Assets.erase(it);
        }
    }
    
    void AssetManager::UnloadAsset(const std::string& path) {
        auto it = m_PathToAssetMap.find(path);
        if (it != m_PathToAssetMap.end()) {
            auto asset = it->second;
            m_Assets.erase(asset->GetID());
            m_PathToAssetMap.erase(it);
        }
    }
    
    void AssetManager::UnloadAllAssets() {
        m_Assets.clear();
        m_PathToAssetMap.clear();
    }
    
}