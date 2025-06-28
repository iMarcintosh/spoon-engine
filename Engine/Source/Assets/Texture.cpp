#include "SpoonEngine/Assets/Texture.h"
#include "SpoonEngine/Renderer/Renderer.h"
#include "SpoonEngine/Renderer/OpenGL/OpenGLTexture.h"

namespace SpoonEngine {
    
    Texture::Texture(AssetID id, const std::string& path)
        : Asset(id, path) {
    }
    
    Texture::Texture(AssetID id, const std::string& path, const TextureSpecification& specification)
        : Asset(id, path), m_Specification(specification) {
    }
    
    Texture::~Texture() {
    }
    
    std::shared_ptr<Texture> Texture::Create(const TextureSpecification& specification) {
        switch (Renderer::GetAPI()) {
            case RendererAPI::None:    return nullptr;
            case RendererAPI::OpenGL:  return std::make_shared<OpenGLTexture2D>(0, "", specification);
        }
        return nullptr;
    }
    
    std::shared_ptr<Texture> Texture::Create(const std::string& path) {
        return AssetManager::GetInstance().LoadAsset<OpenGLTexture2D>(path);
    }
    
    Texture2D::Texture2D(AssetID id, const std::string& path)
        : Texture(id, path) {
    }
    
    Texture2D::Texture2D(AssetID id, const std::string& path, const TextureSpecification& specification)
        : Texture(id, path, specification) {
    }
    
    std::shared_ptr<Texture2D> Texture2D::Create(const TextureSpecification& specification) {
        switch (Renderer::GetAPI()) {
            case RendererAPI::None:    return nullptr;
            case RendererAPI::OpenGL:  return std::make_shared<OpenGLTexture2D>(0, "", specification);
        }
        return nullptr;
    }
    
    std::shared_ptr<Texture2D> Texture2D::Create(const std::string& path) {
        return AssetManager::GetInstance().LoadAsset<OpenGLTexture2D>(path);
    }
    
}