#pragma once

#include "Entity.h"
#include "Component.h"
#include "System.h"
#include <memory>
#include <queue>

namespace SpoonEngine {
    
    class EntityManager {
    private:
        static const uint32_t MAX_ENTITIES = 5000;
        
    public:
        EntityManager() {
            for (EntityID entity = 1; entity < MAX_ENTITIES; ++entity) {
                m_AvailableEntities.push(entity);
            }
        }
        
        EntityID CreateEntity() {
            if (m_LivingEntityCount >= MAX_ENTITIES) {
                throw std::runtime_error("Too many entities in existence");
            }
            
            EntityID id = m_AvailableEntities.front();
            m_AvailableEntities.pop();
            ++m_LivingEntityCount;
            
            return id;
        }
        
        void DestroyEntity(EntityID entity) {
            if (entity >= MAX_ENTITIES) {
                throw std::runtime_error("Entity out of range");
            }
            
            m_Signatures[entity].reset();
            m_AvailableEntities.push(entity);
            --m_LivingEntityCount;
        }
        
        void SetSignature(EntityID entity, Signature signature) {
            if (entity >= MAX_ENTITIES) {
                throw std::runtime_error("Entity out of range");
            }
            
            m_Signatures[entity] = signature;
        }
        
        Signature GetSignature(EntityID entity) {
            if (entity >= MAX_ENTITIES) {
                throw std::runtime_error("Entity out of range");
            }
            
            return m_Signatures[entity];
        }
        
    private:
        std::queue<EntityID> m_AvailableEntities{};
        std::array<Signature, MAX_ENTITIES> m_Signatures{};
        uint32_t m_LivingEntityCount{};
    };
    
    class World {
    public:
        World();
        ~World();
        
        Entity CreateEntity();
        void DestroyEntity(Entity entity);
        
        template<typename T>
        void RegisterComponent() {
            m_ComponentManager->RegisterComponent<T>();
        }
        
        template<typename T>
        void AddComponent(Entity entity, T component) {
            m_ComponentManager->AddComponent<T>(entity, component);
            
            auto signature = m_EntityManager->GetSignature(entity);
            signature.set(m_ComponentManager->GetComponentType<T>(), true);
            m_EntityManager->SetSignature(entity, signature);
            
            m_SystemManager->EntitySignatureChanged(entity, signature);
        }
        
        template<typename T>
        void RemoveComponent(Entity entity) {
            m_ComponentManager->RemoveComponent<T>(entity);
            
            auto signature = m_EntityManager->GetSignature(entity);
            signature.set(m_ComponentManager->GetComponentType<T>(), false);
            m_EntityManager->SetSignature(entity, signature);
            
            m_SystemManager->EntitySignatureChanged(entity, signature);
        }
        
        template<typename T>
        T& GetComponent(Entity entity) {
            return m_ComponentManager->GetComponent<T>(entity);
        }
        
        template<typename T>
        bool HasComponent(Entity entity) {
            return m_ComponentManager->HasComponent<T>(entity);
        }
        
        template<typename T>
        std::shared_ptr<T> RegisterSystem() {
            return m_SystemManager->RegisterSystem<T>();
        }
        
        template<typename T>
        void SetSystemSignature(Signature signature) {
            m_SystemManager->SetSignature<T>(signature);
        }
        
        ComponentManager* GetComponentManager() { return m_ComponentManager.get(); }
        
        void Update();
        void Render(class Renderer* renderer);
        
    private:
        std::unique_ptr<ComponentManager> m_ComponentManager;
        std::unique_ptr<EntityManager> m_EntityManager;
        std::unique_ptr<SystemManager> m_SystemManager;
    };
    
}