#include "SpoonEngine/Renderer/Shader.h"
#include "SpoonEngine/Renderer/Renderer.h"
#include "SpoonEngine/Renderer/OpenGL/OpenGLShader.h"

namespace SpoonEngine {
    
    std::shared_ptr<Shader> Shader::Create(const std::string& filepath) {
        switch (Renderer::GetAPI()) {
            case RendererAPI::None:    return nullptr;
            case RendererAPI::OpenGL:  return std::make_shared<OpenGLShader>(filepath);
        }
        return nullptr;
    }
    
    std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {
        switch (Renderer::GetAPI()) {
            case RendererAPI::None:    return nullptr;
            case RendererAPI::OpenGL:  return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
        }
        return nullptr;
    }
    
    void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader) {
        if (Exists(name)) {
            return;
        }
        m_Shaders[name] = shader;
    }
    
    void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader) {
        auto& name = shader->GetName();
        Add(name, shader);
    }
    
    std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& filepath) {
        auto shader = Shader::Create(filepath);
        Add(shader);
        return shader;
    }
    
    std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath) {
        auto shader = Shader::Create(filepath);
        Add(name, shader);
        return shader;
    }
    
    std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name) {
        if (!Exists(name)) {
            return nullptr;
        }
        return m_Shaders[name];
    }
    
    bool ShaderLibrary::Exists(const std::string& name) const {
        return m_Shaders.find(name) != m_Shaders.end();
    }
    
}