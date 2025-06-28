#pragma once

#include <memory>
#include <string>

namespace SpoonEngine {
    
    enum class RendererAPI {
        None = 0,
        OpenGL = 1,
        Vulkan = 2,
        DirectX11 = 3,
        DirectX12 = 4,
        Metal = 5
    };
    
    class RendererBackend {
    public:
        virtual ~RendererBackend() = default;
        
        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;
        
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void SetClearColor(float r, float g, float b, float a) = 0;
        virtual void Clear() = 0;
        
        virtual void DrawIndexed(uint32_t indexCount) = 0;
        virtual void Present() = 0;
        
        static RendererAPI GetAPI() { return s_API; }
        static void SetAPI(RendererAPI api) { s_API = api; }
        
    private:
        static RendererAPI s_API;
    };
    
    class Renderer {
    public:
        Renderer();
        ~Renderer();
        
        void Initialize();
        void Shutdown();
        
        void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
        void SetClearColor(float r, float g, float b, float a);
        void Clear();
        
        void DrawIndexed(uint32_t indexCount);
        void Present();
        
        static RendererAPI GetAPI() { return RendererBackend::GetAPI(); }
        
    private:
        std::unique_ptr<RendererBackend> m_Backend;
    };
    
}