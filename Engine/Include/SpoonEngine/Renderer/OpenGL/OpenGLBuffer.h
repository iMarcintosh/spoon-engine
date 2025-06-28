#pragma once

#include "SpoonEngine/Renderer/Buffer.h"

namespace SpoonEngine {
    
    class OpenGLVertexBuffer : public VertexBuffer {
    public:
        OpenGLVertexBuffer(uint32_t size);
        OpenGLVertexBuffer(float* vertices, uint32_t size);
        virtual ~OpenGLVertexBuffer();
        
        void Bind() const override;
        void Unbind() const override;
        
        void SetData(const void* data, uint32_t size) override;
        
        const BufferLayout& GetLayout() const override { return m_Layout; }
        void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
        
    private:
        uint32_t m_RendererID;
        BufferLayout m_Layout;
    };
    
    class OpenGLIndexBuffer : public IndexBuffer {
    public:
        OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
        virtual ~OpenGLIndexBuffer();
        
        void Bind() const override;
        void Unbind() const override;
        
        uint32_t GetCount() const override { return m_Count; }
        
    private:
        uint32_t m_RendererID;
        uint32_t m_Count;
    };
    
    class OpenGLVertexArray {
    public:
        OpenGLVertexArray();
        ~OpenGLVertexArray();
        
        void Bind() const;
        void Unbind() const;
        
        void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
        void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);
        
        const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
        const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
        
    private:
        uint32_t m_RendererID;
        uint32_t m_VertexBufferIndex = 0;
        std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
    };
    
}