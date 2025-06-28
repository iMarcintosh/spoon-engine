#pragma once

#include <memory>
#include <string>

namespace SpoonEngine {
    
    class Window;
    class Renderer;
    class World;
    
    class Application {
    public:
        Application(const std::string& name = "SpoonEngine Application");
        virtual ~Application();
        
        void Run();
        void Shutdown();
        
        Window* GetWindow() const { return m_Window.get(); }
        Renderer* GetRenderer() const { return m_Renderer.get(); }
        World* GetWorld() const { return m_World.get(); }
        
    protected:
        virtual void OnInitialize() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender() {}
        virtual void OnShutdown() {}
        
    private:
        void Initialize();
        void Update();
        void Render();
        
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<Renderer> m_Renderer;
        std::unique_ptr<World> m_World;
        
        bool m_Running = false;
        float m_LastFrameTime = 0.0f;
        std::string m_Name;
    };
    
    Application* CreateApplication();
    
}