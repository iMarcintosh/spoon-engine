#pragma once

#include "SpoonEngine/Renderer/Renderer.h"

namespace SpoonEngine {
    
    class OpenGLRenderer : public RendererBackend {
    public:
        void Initialize() override;
        void Shutdown() override;
        
        void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        void SetClearColor(float r, float g, float b, float a) override;
        void Clear() override;
        
        void DrawIndexed(uint32_t indexCount) override;
        void Present() override;
    };
    
}