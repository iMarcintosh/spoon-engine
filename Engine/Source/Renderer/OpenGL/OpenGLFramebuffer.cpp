#include "SpoonEngine/Renderer/OpenGL/OpenGLFramebuffer.h"
#include "SpoonEngine/Renderer/OpenGL/OpenGLTexture.h"

#ifdef SPOON_PLATFORM_MACOS
    #define GL_SILENCE_DEPRECATION
    #include <OpenGL/gl3.h>
#else
    #include <GL/glew.h>
#endif

#include <iostream>

namespace SpoonEngine {
    
    static const uint32_t s_MaxFramebufferSize = 8192;
    
    namespace Utils {
        
        static GLenum TextureTarget(bool multisampled) {
            return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        }
        
        static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count) {
            glGenTextures(count, outID);
        }
        
        static void BindTexture(bool multisampled, uint32_t id) {
            glBindTexture(TextureTarget(multisampled), id);
        }
        
        static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index) {
            bool multisampled = samples > 1;
            if (multisampled) {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
            } else {
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
                
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
        }
        
        static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height) {
            bool multisampled = samples > 1;
            if (multisampled) {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
            } else {
                // Use glTexImage2D instead of glTexStorage2D for better compatibility
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
                
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
        }
        
        static bool IsDepthFormat(FramebufferTextureFormat format) {
            switch (format) {
                case FramebufferTextureFormat::DEPTH24STENCIL8: return true;
                default: return false;
            }
        }
        
        static GLenum SpoonFBTextureFormatToGL(FramebufferTextureFormat format) {
            switch (format) {
                case FramebufferTextureFormat::None:            return 0;
                case FramebufferTextureFormat::RGBA8:           return GL_RGBA8;
                case FramebufferTextureFormat::RED_INTEGER:     return GL_RED_INTEGER;
                case FramebufferTextureFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
            }
            
            return 0;
        }
        
    }
    
    OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
        : m_Specification(spec) {
        
        for (auto spec : m_Specification.Attachments.Attachments) {
            if (!Utils::IsDepthFormat(spec.TextureFormat))
                m_ColorAttachmentSpecs.emplace_back(spec);
            else
                m_DepthAttachmentSpec = spec;
        }
        
        Invalidate();
    }
    
    OpenGLFramebuffer::~OpenGLFramebuffer() {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
        glDeleteTextures(1, &m_DepthAttachment);
    }
    
    void OpenGLFramebuffer::Invalidate() {
        if (m_RendererID) {
            glDeleteFramebuffers(1, &m_RendererID);
            glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
            glDeleteTextures(1, &m_DepthAttachment);
            
            m_ColorAttachments.clear();
            m_DepthAttachment = 0;
            m_ColorAttachmentTextures.clear();
            m_DepthAttachmentTexture = nullptr;
        }
        
        glGenFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        
        bool multisample = m_Specification.Samples > 1;
        
        // Attachments
        if (m_ColorAttachmentSpecs.size()) {
            m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());
            m_ColorAttachmentTextures.resize(m_ColorAttachmentSpecs.size());
            Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());
            
            for (size_t i = 0; i < m_ColorAttachments.size(); i++) {
                Utils::BindTexture(multisample, m_ColorAttachments[i]);
                
                switch (m_ColorAttachmentSpecs[i].TextureFormat) {
                    case FramebufferTextureFormat::None:
                        break;
                    case FramebufferTextureFormat::RGBA8:
                        Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
                        break;
                    case FramebufferTextureFormat::RED_INTEGER:
                        Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
                        break;
                    case FramebufferTextureFormat::DEPTH24STENCIL8:
                        // Depth format in color attachment - should not happen
                        break;
                }
                
                // Create texture wrapper
                TextureSpecification texSpec;
                texSpec.Width = m_Specification.Width;
                texSpec.Height = m_Specification.Height;
                texSpec.Format = TextureFormat::RGBA8; // Default for framebuffer textures
                
                auto texture = std::make_shared<OpenGLTexture2D>(0, "", texSpec);
                texture->m_RendererID = m_ColorAttachments[i];
                texture->m_Width = m_Specification.Width;
                texture->m_Height = m_Specification.Height;
                texture->m_IsLoaded = true;
                m_ColorAttachmentTextures[i] = texture;
            }
        }
        
        if (m_DepthAttachmentSpec.TextureFormat != FramebufferTextureFormat::None) {
            Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
            Utils::BindTexture(multisample, m_DepthAttachment);
            
            switch (m_DepthAttachmentSpec.TextureFormat) {
                case FramebufferTextureFormat::None:
                    break;
                case FramebufferTextureFormat::RGBA8:
                case FramebufferTextureFormat::RED_INTEGER:
                    // Color formats in depth attachment - should not happen
                    break;
                case FramebufferTextureFormat::DEPTH24STENCIL8:
                    Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
                    break;
            }
            
            // Create depth texture wrapper
            TextureSpecification depthSpec;
            depthSpec.Width = m_Specification.Width;
            depthSpec.Height = m_Specification.Height;
            depthSpec.Format = TextureFormat::RGBA8; // Placeholder
            
            auto depthTexture = std::make_shared<OpenGLTexture2D>(0, "", depthSpec);
            depthTexture->m_RendererID = m_DepthAttachment;
            depthTexture->m_Width = m_Specification.Width;
            depthTexture->m_Height = m_Specification.Height;
            depthTexture->m_IsLoaded = true;
            m_DepthAttachmentTexture = depthTexture;
        }
        
        if (m_ColorAttachments.size() > 1) {
            if (m_ColorAttachments.size() <= 4) {
                GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
                glDrawBuffers(m_ColorAttachments.size(), buffers);
            }
        } else if (m_ColorAttachments.empty()) {
            // Only depth-pass
            glDrawBuffer(GL_NONE);
        }
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer not complete!" << std::endl;
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void OpenGLFramebuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Specification.Width, m_Specification.Height);
    }
    
    void OpenGLFramebuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height) {
        if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize) {
            std::cout << "Attempted to resize framebuffer to " << width << ", " << height << std::endl;
            return;
        }
        
        m_Specification.Width = width;
        m_Specification.Height = height;
        
        Invalidate();
    }
    
    int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y) {
        if (attachmentIndex >= m_ColorAttachments.size())
            return -1;
        
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        int pixelData;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
        return pixelData;
    }
    
    void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value) {
        if (attachmentIndex >= m_ColorAttachments.size())
            return;
        
        // Alternative implementation without glClearTexImage for compatibility
        Bind();
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        glClearColor(static_cast<float>(value), static_cast<float>(value), static_cast<float>(value), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Unbind();
    }
    
}