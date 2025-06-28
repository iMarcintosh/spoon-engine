#pragma once

#include "SpoonEngine/ECS/System.h"
#include "SpoonEngine/Components/Transform.h"
#include <memory>

namespace SpoonEngine {
    
    class RenderSystem : public System {
    public:
        RenderSystem();
        
        void SetCamera(std::shared_ptr<class Camera> camera) { m_Camera = camera; }
        std::shared_ptr<class Camera> GetCamera() const { return m_Camera; }
        
        void Update(float deltaTime) override;
        void Render(class Renderer* renderer) override;
        
        void SetWorld(class World* world) { m_World = world; }
        class World* GetWorld() const { return m_World; }
        
    private:
        std::shared_ptr<class Camera> m_Camera;
        class World* m_World = nullptr;
    };
    
    class CameraSystem : public System {
    public:
        CameraSystem();
        
        void Update(float deltaTime) override;
        void Render(class Renderer* renderer) override;
        
        void SetWorld(class World* world) { m_World = world; }
        class World* GetWorld() const { return m_World; }
        
        std::shared_ptr<class Camera> GetActiveCamera() const { return m_ActiveCamera; }
        
    private:
        void UpdateActiveCamera();
        
        std::shared_ptr<class Camera> m_ActiveCamera;
        class World* m_World = nullptr;
    };
    
}