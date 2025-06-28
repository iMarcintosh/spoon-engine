#pragma once

#include "Entity.h"
#include <set>
#include <bitset>
#include <memory>
#include <unordered_map>
#include <stdexcept>

namespace SpoonEngine {
    
    using Signature = std::bitset<32>;
    
    class System {
    public:
        std::set<EntityID> m_Entities;
        
        virtual ~System() = default;
        virtual void Update(float deltaTime) {}
        virtual void Render(class Renderer* renderer) {}
    };
    
    class SystemManager {
    public:
        template<typename T>
        std::shared_ptr<T> RegisterSystem() {
            const char* typeName = typeid(T).name();
            
            if (m_Systems.find(typeName) != m_Systems.end()) {
                return std::static_pointer_cast<T>(m_Systems[typeName]);
            }
            
            auto system = std::make_shared<T>();
            m_Systems.insert({typeName, system});
            return system;
        }
        
        template<typename T>
        void SetSignature(Signature signature) {
            const char* typeName = typeid(T).name();
            
            if (m_Systems.find(typeName) == m_Systems.end()) {
                throw std::runtime_error("System used before registered");
            }
            
            m_Signatures.insert({typeName, signature});
        }
        
        void EntityDestroyed(EntityID entity) {
            for (auto const& pair : m_Systems) {
                auto const& system = pair.second;
                system->m_Entities.erase(entity);
            }
        }
        
        void EntitySignatureChanged(EntityID entity, Signature entitySignature) {
            for (auto const& pair : m_Systems) {
                auto const& type = pair.first;
                auto const& system = pair.second;
                auto const& systemSignature = m_Signatures[type];
                
                if ((entitySignature & systemSignature) == systemSignature) {
                    system->m_Entities.insert(entity);
                } else {
                    system->m_Entities.erase(entity);
                }
            }
        }
        
        void UpdateSystems(float deltaTime) {
            for (auto const& pair : m_Systems) {
                pair.second->Update(deltaTime);
            }
        }
        
        void RenderSystems(class Renderer* renderer) {
            for (auto const& pair : m_Systems) {
                pair.second->Render(renderer);
            }
        }
        
    private:
        std::unordered_map<const char*, Signature> m_Signatures{};
        std::unordered_map<const char*, std::shared_ptr<System>> m_Systems{};
    };
    
}