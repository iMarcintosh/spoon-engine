#include "SpoonEngine/ECS/World.h"

namespace SpoonEngine {
    
    World::World() {
        m_ComponentManager = std::make_unique<ComponentManager>();
        m_EntityManager = std::make_unique<EntityManager>();
        m_SystemManager = std::make_unique<SystemManager>();
    }
    
    World::~World() {
    }
    
    Entity World::CreateEntity() {
        return Entity(m_EntityManager->CreateEntity());
    }
    
    void World::DestroyEntity(Entity entity) {
        m_EntityManager->DestroyEntity(entity);
        m_ComponentManager->EntityDestroyed(entity);
        m_SystemManager->EntityDestroyed(entity);
    }
    
    void World::Update() {
        m_SystemManager->UpdateSystems(0.0f);
    }
    
    void World::Render(Renderer* renderer) {
        m_SystemManager->RenderSystems(renderer);
    }
    
}