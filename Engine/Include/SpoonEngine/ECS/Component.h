#pragma once

#include <typeindex>
#include <unordered_map>
#include <memory>
#include <vector>
#include <array>
#include <stdexcept>
#include "Entity.h"

namespace SpoonEngine {
    
    using ComponentTypeID = std::type_index;
    
    template<typename T>
    ComponentTypeID GetComponentTypeID() {
        return std::type_index(typeid(T));
    }
    
    class IComponentArray {
    public:
        virtual ~IComponentArray() = default;
        virtual void EntityDestroyed(EntityID entity) = 0;
    };
    
    template<typename T>
    class ComponentArray : public IComponentArray {
    public:
        void InsertData(EntityID entity, T component) {
            if (m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end()) {
                return;
            }
            
            size_t newIndex = m_Size;
            m_EntityToIndexMap[entity] = newIndex;
            m_IndexToEntityMap[newIndex] = entity;
            m_ComponentArray[newIndex] = component;
            ++m_Size;
        }
        
        void RemoveData(EntityID entity) {
            if (m_EntityToIndexMap.find(entity) == m_EntityToIndexMap.end()) {
                return;
            }
            
            size_t indexOfRemovedEntity = m_EntityToIndexMap[entity];
            size_t indexOfLastElement = m_Size - 1;
            m_ComponentArray[indexOfRemovedEntity] = m_ComponentArray[indexOfLastElement];
            
            EntityID entityOfLastElement = m_IndexToEntityMap[indexOfLastElement];
            m_EntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
            m_IndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;
            
            m_EntityToIndexMap.erase(entity);
            m_IndexToEntityMap.erase(indexOfLastElement);
            
            --m_Size;
        }
        
        T& GetData(EntityID entity) {
            if (m_EntityToIndexMap.find(entity) == m_EntityToIndexMap.end()) {
                throw std::runtime_error("Entity does not have this component");
            }
            return m_ComponentArray[m_EntityToIndexMap[entity]];
        }
        
        bool HasData(EntityID entity) const {
            return m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end();
        }
        
        void EntityDestroyed(EntityID entity) override {
            if (m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end()) {
                RemoveData(entity);
            }
        }
        
    private:
        std::array<T, 5000> m_ComponentArray;
        std::unordered_map<EntityID, size_t> m_EntityToIndexMap;
        std::unordered_map<size_t, EntityID> m_IndexToEntityMap;
        size_t m_Size = 0;
    };
    
    class ComponentManager {
    public:
        template<typename T>
        void RegisterComponent() {
            ComponentTypeID typeID = GetComponentTypeID<T>();
            
            if (m_ComponentTypes.find(typeID) != m_ComponentTypes.end()) {
                return;
            }
            
            m_ComponentTypes.insert({typeID, m_NextComponentType});
            m_ComponentArrays.insert({typeID, std::make_shared<ComponentArray<T>>()});
            
            ++m_NextComponentType;
        }
        
        template<typename T>
        uint8_t GetComponentType() {
            ComponentTypeID typeID = GetComponentTypeID<T>();
            
            if (m_ComponentTypes.find(typeID) == m_ComponentTypes.end()) {
                throw std::runtime_error("Component not registered before use");
            }
            
            return m_ComponentTypes[typeID];
        }
        
        template<typename T>
        void AddComponent(EntityID entity, T component) {
            GetComponentArray<T>()->InsertData(entity, component);
        }
        
        template<typename T>
        void RemoveComponent(EntityID entity) {
            GetComponentArray<T>()->RemoveData(entity);
        }
        
        template<typename T>
        T& GetComponent(EntityID entity) {
            return GetComponentArray<T>()->GetData(entity);
        }
        
        template<typename T>
        bool HasComponent(EntityID entity) {
            return GetComponentArray<T>()->HasData(entity);
        }
        
        void EntityDestroyed(EntityID entity) {
            for (auto const& pair : m_ComponentArrays) {
                auto const& component = pair.second;
                component->EntityDestroyed(entity);
            }
        }
        
    private:
        std::unordered_map<ComponentTypeID, uint8_t> m_ComponentTypes{};
        std::unordered_map<ComponentTypeID, std::shared_ptr<IComponentArray>> m_ComponentArrays{};
        uint8_t m_NextComponentType{};
        
        template<typename T>
        std::shared_ptr<ComponentArray<T>> GetComponentArray() {
            ComponentTypeID typeID = GetComponentTypeID<T>();
            
            if (m_ComponentTypes.find(typeID) == m_ComponentTypes.end()) {
                throw std::runtime_error("Component not registered before use");
            }
            
            return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[typeID]);
        }
    };
    
}