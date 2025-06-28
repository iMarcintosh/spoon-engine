#include "SpoonEngine/Renderer/Framebuffer.h"
#include "SpoonEngine/Renderer/Renderer.h"
#include "SpoonEngine/Renderer/OpenGL/OpenGLFramebuffer.h"

namespace SpoonEngine {
    
    std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec) {
        switch (Renderer::GetAPI()) {
            case RendererAPI::None:    return nullptr;
            case RendererAPI::OpenGL:  return std::make_shared<OpenGLFramebuffer>(spec);
            case RendererAPI::Vulkan:  return nullptr;
            case RendererAPI::DirectX11: return nullptr;
            case RendererAPI::DirectX12: return nullptr;
            case RendererAPI::Metal:   return nullptr;
        }
        
        return nullptr;
    }
    
}