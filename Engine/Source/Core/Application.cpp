#include "SpoonEngine/Core/Application.h"
#include "SpoonEngine/Core/Window.h"
#include "SpoonEngine/Renderer/Renderer.h"
#include "SpoonEngine/ECS/World.h"

#include <GLFW/glfw3.h>
#include <iostream>

namespace SpoonEngine {
    
    Application::Application(const std::string& name)
        : m_Name(name) {
    }
    
    Application::~Application() {
    }
    
    void Application::Run() {
        Initialize();
        
        m_Running = true;
        while (m_Running) {
            float time = static_cast<float>(glfwGetTime());
            float deltaTime = time - m_LastFrameTime;
            m_LastFrameTime = time;
            
            Update();
            OnUpdate(deltaTime);
            
            Render();
            OnRender();
            
            m_Window->OnUpdate();
            
            if (m_Window->ShouldClose()) {
                m_Running = false;
            }
        }
        
        OnShutdown();
        Shutdown();
    }
    
    void Application::Shutdown() {
        m_Running = false;
    }
    
    void Application::Initialize() {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW!" << std::endl;
            return;
        }
        
        WindowProps props(m_Name);
        m_Window = std::make_unique<Window>(props);
        
        m_Renderer = std::make_unique<Renderer>();
        m_Renderer->Initialize();
        m_Renderer->SetViewport(0, 0, m_Window->GetWidth(), m_Window->GetHeight());
        
        m_World = std::make_unique<World>();
        
        OnInitialize();
    }
    
    void Application::Update() {
        if (m_World) {
            m_World->Update();
        }
    }
    
    void Application::Render() {
        if (m_Renderer) {
            m_Renderer->Clear();
            
            if (m_World) {
                m_World->Render(m_Renderer.get());
            }
            
            m_Renderer->Present();
        }
    }
    
}