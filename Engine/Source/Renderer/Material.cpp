#include "SpoonEngine/Renderer/Material.h"
#include "SpoonEngine/Renderer/Shader.h"
#include "SpoonEngine/Assets/Texture.h"

namespace SpoonEngine {
    
    Material::Material(const std::string& name)
        : m_Name(name) {
    }
    
    Material::Material(std::shared_ptr<Shader> shader, const std::string& name)
        : m_Name(name), m_Shader(shader) {
    }
    
    void Material::Bind() {
        if (m_Shader) {
            m_Shader->Bind();
            
            // Bind textures
            uint32_t textureSlot = 0;
            for (const auto& [name, texture] : m_Textures) {
                if (texture && texture->IsLoaded()) {
                    texture->Bind(textureSlot);
                    m_Shader->SetInt(name, textureSlot);
                    textureSlot++;
                }
            }
            
            // Set uniform values
            for (const auto& [name, value] : m_FloatUniforms) {
                m_Shader->SetFloat(name, value);
            }
            
            for (const auto& [name, value] : m_Float2Uniforms) {
                m_Shader->SetFloat2(name, value);
            }
            
            for (const auto& [name, value] : m_Float3Uniforms) {
                m_Shader->SetFloat3(name, value);
            }
            
            for (const auto& [name, value] : m_Float4Uniforms) {
                m_Shader->SetFloat4(name, value);
            }
            
            for (const auto& [name, value] : m_IntUniforms) {
                m_Shader->SetInt(name, value);
            }
            
            for (const auto& [name, value] : m_Mat4Uniforms) {
                m_Shader->SetMat4(name, value);
            }
        }
    }
    
    void Material::Unbind() {
        if (m_Shader) {
            m_Shader->Unbind();
        }
        
        // Unbind textures
        for (const auto& [name, texture] : m_Textures) {
            if (texture && texture->IsLoaded()) {
                texture->Unbind();
            }
        }
    }
    
    void Material::SetTexture(const std::string& name, std::shared_ptr<Texture> texture) {
        m_Textures[name] = texture;
    }
    
    std::shared_ptr<Texture> Material::GetTexture(const std::string& name) const {
        auto it = m_Textures.find(name);
        return (it != m_Textures.end()) ? it->second : nullptr;
    }
    
    void Material::SetFloat(const std::string& name, float value) {
        m_FloatUniforms[name] = value;
    }
    
    void Material::SetFloat2(const std::string& name, const glm::vec2& value) {
        m_Float2Uniforms[name] = value;
    }
    
    void Material::SetFloat3(const std::string& name, const glm::vec3& value) {
        m_Float3Uniforms[name] = value;
    }
    
    void Material::SetFloat4(const std::string& name, const glm::vec4& value) {
        m_Float4Uniforms[name] = value;
    }
    
    void Material::SetInt(const std::string& name, int value) {
        m_IntUniforms[name] = value;
    }
    
    void Material::SetMat4(const std::string& name, const glm::mat4& value) {
        m_Mat4Uniforms[name] = value;
    }
    
    float Material::GetFloat(const std::string& name) const {
        auto it = m_FloatUniforms.find(name);
        return (it != m_FloatUniforms.end()) ? it->second : 0.0f;
    }
    
    glm::vec2 Material::GetFloat2(const std::string& name) const {
        auto it = m_Float2Uniforms.find(name);
        return (it != m_Float2Uniforms.end()) ? it->second : glm::vec2(0.0f);
    }
    
    glm::vec3 Material::GetFloat3(const std::string& name) const {
        auto it = m_Float3Uniforms.find(name);
        return (it != m_Float3Uniforms.end()) ? it->second : glm::vec3(0.0f);
    }
    
    glm::vec4 Material::GetFloat4(const std::string& name) const {
        auto it = m_Float4Uniforms.find(name);
        return (it != m_Float4Uniforms.end()) ? it->second : glm::vec4(0.0f);
    }
    
    int Material::GetInt(const std::string& name) const {
        auto it = m_IntUniforms.find(name);
        return (it != m_IntUniforms.end()) ? it->second : 0;
    }
    
    glm::mat4 Material::GetMat4(const std::string& name) const {
        auto it = m_Mat4Uniforms.find(name);
        return (it != m_Mat4Uniforms.end()) ? it->second : glm::mat4(1.0f);
    }
    
    std::shared_ptr<Material> Material::Create(const std::string& name) {
        return std::make_shared<Material>(name);
    }
    
    std::shared_ptr<Material> Material::Create(std::shared_ptr<Shader> shader, const std::string& name) {
        return std::make_shared<Material>(shader, name);
    }
    
    // MaterialLibrary implementation
    void MaterialLibrary::Add(std::shared_ptr<Material> material) {
        m_Materials[material->GetName()] = material;
    }
    
    std::shared_ptr<Material> MaterialLibrary::Get(const std::string& name) {
        auto it = m_Materials.find(name);
        return (it != m_Materials.end()) ? it->second : nullptr;
    }
    
    std::shared_ptr<Material> MaterialLibrary::Load(const std::string& name, std::shared_ptr<Shader> shader) {
        auto material = Material::Create(shader, name);
        Add(material);
        return material;
    }
    
    bool MaterialLibrary::Exists(const std::string& name) const {
        return m_Materials.find(name) != m_Materials.end();
    }
    
    void MaterialLibrary::Remove(const std::string& name) {
        m_Materials.erase(name);
    }
    
    void MaterialLibrary::Clear() {
        m_Materials.clear();
    }
    
}