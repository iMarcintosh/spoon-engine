#pragma once

#include "AssetManager.h"
#include <cstdint>

namespace SpoonEngine {
    
    enum class TextureFormat {
        None = 0,
        R8,
        RGB8,
        RGBA8,
        RGBA32F
    };
    
    struct TextureSpecification {
        uint32_t Width = 1;
        uint32_t Height = 1;
        TextureFormat Format = TextureFormat::RGBA8;
        bool GenerateMips = true;
    };
    
    class Texture : public Asset {
    public:
        Texture(AssetID id, const std::string& path);
        Texture(AssetID id, const std::string& path, const TextureSpecification& specification);
        virtual ~Texture();
        
        virtual bool Load() = 0;
        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual void Unbind() const = 0;
        
        virtual void SetData(void* data, uint32_t size) = 0;
        
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetRendererID() const = 0;
        
        virtual bool IsLoaded() const = 0;
        
        virtual bool operator==(const Texture& other) const = 0;
        
        static std::shared_ptr<Texture> Create(const TextureSpecification& specification);
        static std::shared_ptr<Texture> Create(const std::string& path);
        
    protected:
        TextureSpecification m_Specification;
    };
    
    class Texture2D : public Texture {
    public:
        Texture2D(AssetID id, const std::string& path);
        Texture2D(AssetID id, const std::string& path, const TextureSpecification& specification);
        
        bool Load() override { return false; }
        void Bind(uint32_t slot = 0) const override {}
        void Unbind() const override {}
        void SetData(void* data, uint32_t size) override {}
        uint32_t GetWidth() const override { return 0; }
        uint32_t GetHeight() const override { return 0; }
        uint32_t GetRendererID() const override { return 0; }
        bool IsLoaded() const override { return false; }
        bool operator==(const Texture& other) const override { return false; }
        
        static std::shared_ptr<Texture2D> Create(const TextureSpecification& specification);
        static std::shared_ptr<Texture2D> Create(const std::string& path);
    };
    
}