#pragma once

#include "SpoonEngine/Assets/Texture.h"

namespace SpoonEngine {
    
    class OpenGLTexture2D : public Texture2D {
    public:
        OpenGLTexture2D(AssetID id, const std::string& path);
        OpenGLTexture2D(AssetID id, const std::string& path, const TextureSpecification& specification);
        virtual ~OpenGLTexture2D();
        
        bool Load() override;
        void Bind(uint32_t slot = 0) const override;
        void Unbind() const override;
        
        void SetData(void* data, uint32_t size) override;
        
        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }
        uint32_t GetRendererID() const override { return m_RendererID; }
        
        bool IsLoaded() const override { return m_IsLoaded; }
        
        bool operator==(const Texture& other) const override {
            return m_RendererID == other.GetRendererID();
        }
        
        friend class OpenGLFramebuffer;
        
    private:
        uint32_t m_RendererID = 0;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        uint32_t m_Channels = 0;
        bool m_IsLoaded = false;
        
        uint32_t GetOpenGLFormat(TextureFormat format) const;
    };
    
}