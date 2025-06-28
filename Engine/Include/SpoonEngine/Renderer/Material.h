#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace SpoonEngine {
    
    class Material {
    public:
        Material(const std::string& name = "Material");
        Material(std::shared_ptr<class Shader> shader, const std::string& name = "Material");
        ~Material() = default;
        
        void Bind();
        void Unbind();
        
        void SetShader(std::shared_ptr<class Shader> shader) { m_Shader = shader; }
        std::shared_ptr<class Shader> GetShader() const { return m_Shader; }
        
        // Texture management
        void SetTexture(const std::string& name, std::shared_ptr<class Texture> texture);
        std::shared_ptr<class Texture> GetTexture(const std::string& name) const;
        
        // Uniform setters
        void SetFloat(const std::string& name, float value);
        void SetFloat2(const std::string& name, const glm::vec2& value);
        void SetFloat3(const std::string& name, const glm::vec3& value);
        void SetFloat4(const std::string& name, const glm::vec4& value);
        void SetInt(const std::string& name, int value);
        void SetMat4(const std::string& name, const glm::mat4& value);
        
        // Get uniform values
        float GetFloat(const std::string& name) const;
        glm::vec2 GetFloat2(const std::string& name) const;
        glm::vec3 GetFloat3(const std::string& name) const;
        glm::vec4 GetFloat4(const std::string& name) const;
        int GetInt(const std::string& name) const;
        glm::mat4 GetMat4(const std::string& name) const;
        
        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }
        
        static std::shared_ptr<Material> Create(const std::string& name = "Material");
        static std::shared_ptr<Material> Create(std::shared_ptr<class Shader> shader, const std::string& name = "Material");
        
    private:
        std::string m_Name;
        std::shared_ptr<class Shader> m_Shader;
        
        // Textures
        std::unordered_map<std::string, std::shared_ptr<class Texture>> m_Textures;
        
        // Uniform values
        std::unordered_map<std::string, float> m_FloatUniforms;
        std::unordered_map<std::string, glm::vec2> m_Float2Uniforms;
        std::unordered_map<std::string, glm::vec3> m_Float3Uniforms;
        std::unordered_map<std::string, glm::vec4> m_Float4Uniforms;
        std::unordered_map<std::string, int> m_IntUniforms;
        std::unordered_map<std::string, glm::mat4> m_Mat4Uniforms;
    };
    
    class MaterialLibrary {
    public:
        static MaterialLibrary& GetInstance() {
            static MaterialLibrary instance;
            return instance;
        }
        
        void Add(std::shared_ptr<Material> material);
        std::shared_ptr<Material> Get(const std::string& name);
        std::shared_ptr<Material> Load(const std::string& name, std::shared_ptr<class Shader> shader);
        
        bool Exists(const std::string& name) const;
        void Remove(const std::string& name);
        void Clear();
        
    private:
        MaterialLibrary() = default;
        std::unordered_map<std::string, std::shared_ptr<Material>> m_Materials;
    };
    
}