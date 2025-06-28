#include "SpoonEngine/Renderer/Renderer.h"
#include "SpoonEngine/Renderer/OpenGL/OpenGLRenderer.h"

namespace SpoonEngine {
    
    RendererAPI RendererBackend::s_API = RendererAPI::OpenGL;
    
    Renderer::Renderer() {
    }
    
    Renderer::~Renderer() {
    }
    
    void Renderer::Initialize() {
        switch (RendererBackend::GetAPI()) {
            case RendererAPI::None:
                break;
            case RendererAPI::OpenGL:
                m_Backend = std::make_unique<OpenGLRenderer>();
                break;
            default:
                break;
        }
        
        if (m_Backend) {
            m_Backend->Initialize();
        }
    }
    
    void Renderer::Shutdown() {
        if (m_Backend) {
            m_Backend->Shutdown();
        }
    }
    
    void Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
        if (m_Backend) {
            m_Backend->SetViewport(x, y, width, height);
        }
    }
    
    void Renderer::SetClearColor(float r, float g, float b, float a) {
        if (m_Backend) {
            m_Backend->SetClearColor(r, g, b, a);
        }
    }
    
    void Renderer::Clear() {
        if (m_Backend) {
            m_Backend->Clear();
        }
    }
    
    void Renderer::DrawIndexed(uint32_t indexCount) {
        if (m_Backend) {
            m_Backend->DrawIndexed(indexCount);
        }
    }
    
    void Renderer::Present() {
        if (m_Backend) {
            m_Backend->Present();
        }
    }
    
}