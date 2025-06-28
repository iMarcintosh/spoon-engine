#include "SpoonEngine/Renderer/Buffer.h"
#include "SpoonEngine/Renderer/Renderer.h"
#include "SpoonEngine/Renderer/OpenGL/OpenGLBuffer.h"

namespace SpoonEngine {
    
    std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size) {
        switch (Renderer::GetAPI()) {
            case RendererAPI::None:    return nullptr;
            case RendererAPI::OpenGL:  return std::make_shared<OpenGLVertexBuffer>(size);
        }
        return nullptr;
    }
    
    std::shared_ptr<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size) {
        switch (Renderer::GetAPI()) {
            case RendererAPI::None:    return nullptr;
            case RendererAPI::OpenGL:  return std::make_shared<OpenGLVertexBuffer>(vertices, size);
        }
        return nullptr;
    }
    
    std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count) {
        switch (Renderer::GetAPI()) {
            case RendererAPI::None:    return nullptr;
            case RendererAPI::OpenGL:  return std::make_shared<OpenGLIndexBuffer>(indices, count);
        }
        return nullptr;
    }
    
}