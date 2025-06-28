#pragma once

#include "SpoonEngine/Renderer/Framebuffer.h"

namespace SpoonEngine {
    
    class OpenGLFramebuffer : public Framebuffer {
    public:
        OpenGLFramebuffer(const FramebufferSpecification& spec);
        virtual ~OpenGLFramebuffer();
        
        void Invalidate();
        
        virtual void Bind() override;
        virtual void Unbind() override;
        
        virtual void Resize(uint32_t width, uint32_t height) override;
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
        
        virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;
        
        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override {
            if (index >= m_ColorAttachments.size()) return 0;
            return m_ColorAttachments[index];
        }
        
        virtual std::shared_ptr<Texture2D> GetColorAttachment(uint32_t index = 0) const override {
            if (index >= m_ColorAttachmentTextures.size()) return nullptr;
            return m_ColorAttachmentTextures[index];
        }
        
        virtual std::shared_ptr<Texture2D> GetDepthAttachment() const override {
            return m_DepthAttachmentTexture;
        }
        
        virtual const FramebufferSpecification& GetSpecification() const override {
            return m_Specification;
        }
        
    private:
        uint32_t m_RendererID = 0;
        FramebufferSpecification m_Specification;
        
        std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecs;
        FramebufferTextureSpecification m_DepthAttachmentSpec = FramebufferTextureFormat::None;
        
        std::vector<uint32_t> m_ColorAttachments;
        uint32_t m_DepthAttachment = 0;
        
        std::vector<std::shared_ptr<Texture2D>> m_ColorAttachmentTextures;
        std::shared_ptr<Texture2D> m_DepthAttachmentTexture;
    };
    
}