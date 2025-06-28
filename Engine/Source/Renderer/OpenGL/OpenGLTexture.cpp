#include "SpoonEngine/Renderer/OpenGL/OpenGLTexture.h"

#ifdef SPOON_PLATFORM_MACOS
    #define GL_SILENCE_DEPRECATION
    #include <OpenGL/gl3.h>
#else
    #include <GL/glew.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "../../../../ThirdParty/stb_image.h"

#include <iostream>

namespace SpoonEngine {
    
    OpenGLTexture2D::OpenGLTexture2D(AssetID id, const std::string& path)
        : Texture2D(id, path) {
    }
    
    OpenGLTexture2D::OpenGLTexture2D(AssetID id, const std::string& path, const TextureSpecification& specification)
        : Texture2D(id, path, specification) {
    }
    
    OpenGLTexture2D::~OpenGLTexture2D() {
        if (m_RendererID) {
            glDeleteTextures(1, &m_RendererID);
        }
    }
    
    bool OpenGLTexture2D::Load() {
        if (m_IsLoaded) return true;
        
        const std::string& path = GetPath();
        if (path.empty()) {
            // Create empty texture from specification
            glGenTextures(1, &m_RendererID);
            glBindTexture(GL_TEXTURE_2D, m_RendererID);
            
            m_Width = m_Specification.Width;
            m_Height = m_Specification.Height;
            
            uint32_t internalFormat = GetOpenGLFormat(m_Specification.Format);
            uint32_t dataFormat = GL_RGBA;
            
            if (m_Specification.Format == TextureFormat::RGB8) {
                dataFormat = GL_RGB;
            } else if (m_Specification.Format == TextureFormat::R8) {
                dataFormat = GL_RED;
            }
            
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, nullptr);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            
            m_IsLoaded = true;
            return true;
        }
        
        // Load texture from file
        stbi_set_flip_vertically_on_load(1);
        
        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        
        if (!data) {
            std::cout << "Failed to load texture: " << path << std::endl;
            return false;
        }
        
        m_Width = width;
        m_Height = height;
        m_Channels = channels;
        
        uint32_t internalFormat = 0;
        uint32_t dataFormat = 0;
        
        if (channels == 4) {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        } else if (channels == 3) {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        } else if (channels == 1) {
            internalFormat = GL_R8;
            dataFormat = GL_RED;
        }
        
        if (internalFormat == 0 || dataFormat == 0) {
            std::cout << "Unsupported texture format with " << channels << " channels: " << path << std::endl;
            stbi_image_free(data);
            return false;
        }
        
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        
        if (m_Specification.GenerateMips) {
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
        stbi_image_free(data);
        m_IsLoaded = true;
        
        std::cout << "Loaded texture: " << path << " (" << width << "x" << height << ", " << channels << " channels)" << std::endl;
        return true;
    }
    
    void OpenGLTexture2D::Bind(uint32_t slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }
    
    void OpenGLTexture2D::Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    void OpenGLTexture2D::SetData(void* data, uint32_t size) {
        uint32_t bytesPerPixel = 0;
        uint32_t dataFormat = 0;
        
        if (m_Specification.Format == TextureFormat::RGBA8) {
            bytesPerPixel = 4;
            dataFormat = GL_RGBA;
        } else if (m_Specification.Format == TextureFormat::RGB8) {
            bytesPerPixel = 3;
            dataFormat = GL_RGB;
        } else if (m_Specification.Format == TextureFormat::R8) {
            bytesPerPixel = 1;
            dataFormat = GL_RED;
        }
        
        if (size != m_Width * m_Height * bytesPerPixel) {
            std::cout << "Data size mismatch for texture!" << std::endl;
            return;
        }
        
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    uint32_t OpenGLTexture2D::GetOpenGLFormat(TextureFormat format) const {
        switch (format) {
            case TextureFormat::R8:     return GL_R8;
            case TextureFormat::RGB8:   return GL_RGB8;
            case TextureFormat::RGBA8:  return GL_RGBA8;
            case TextureFormat::RGBA32F: return GL_RGBA32F;
            default: return GL_RGBA8;
        }
    }
    
}