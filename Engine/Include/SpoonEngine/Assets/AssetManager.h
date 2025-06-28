#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <typeindex>

namespace SpoonEngine {
    
    using AssetID = uint32_t;
    const AssetID INVALID_ASSET = 0;
    
    class Asset {
    public:
        Asset(AssetID id, const std::string& path) : m_ID(id), m_Path(path) {}
        virtual ~Asset() = default;
        
        virtual bool Load() { return false; }
        
        AssetID GetID() const { return m_ID; }
        const std::string& GetPath() const { return m_Path; }
        
    private:
        AssetID m_ID;
        std::string m_Path;
    };
    
    class AssetManager {
    public:
        static AssetManager& GetInstance() {
            static AssetManager instance;
            return instance;
        }
        
        template<typename T>
        std::shared_ptr<T> LoadAsset(const std::string& path) {
            auto it = m_PathToAssetMap.find(path);
            if (it != m_PathToAssetMap.end()) {
                return std::static_pointer_cast<T>(it->second);
            }
            
            AssetID newID = GenerateAssetID();
            auto asset = std::make_shared<T>(newID, path);
            
            if (asset->Load()) {
                m_Assets[newID] = asset;
                m_PathToAssetMap[path] = asset;
                return asset;
            }
            
            return nullptr;
        }
        
        template<typename T>
        std::shared_ptr<T> GetAsset(AssetID id) {
            auto it = m_Assets.find(id);
            if (it != m_Assets.end()) {
                return std::static_pointer_cast<T>(it->second);
            }
            return nullptr;
        }
        
        template<typename T>
        std::shared_ptr<T> GetAsset(const std::string& path) {
            auto it = m_PathToAssetMap.find(path);
            if (it != m_PathToAssetMap.end()) {
                return std::static_pointer_cast<T>(it->second);
            }
            return nullptr;
        }
        
        void UnloadAsset(AssetID id);
        void UnloadAsset(const std::string& path);
        void UnloadAllAssets();
        
    private:
        AssetManager() = default;
        
        AssetID GenerateAssetID() {
            return ++m_NextAssetID;
        }
        
        AssetID m_NextAssetID = 0;
        std::unordered_map<AssetID, std::shared_ptr<Asset>> m_Assets;
        std::unordered_map<std::string, std::shared_ptr<Asset>> m_PathToAssetMap;
    };
    
}