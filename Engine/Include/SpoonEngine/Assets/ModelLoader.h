#pragma once

#include "SpoonEngine/Assets/Model.h"
#include "SpoonEngine/Math/Vector3.h"
#include "SpoonEngine/Math/Vector2.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace SpoonEngine {
    
    struct ModelVertex {
        Vector3 Position;
        Vector3 Normal;
        Vector2 TexCoords;
        Vector3 Tangent = Vector3::Zero();
        Vector3 Bitangent = Vector3::Zero();
        
        ModelVertex() = default;
        ModelVertex(const Vector3& pos, const Vector3& norm, const Vector2& tex)
            : Position(pos), Normal(norm), TexCoords(tex) {}
    };
    
    struct ModelMaterial {
        std::string Name;
        Vector3 Ambient = Vector3(0.2f, 0.2f, 0.2f);
        Vector3 Diffuse = Vector3(0.8f, 0.8f, 0.8f);
        Vector3 Specular = Vector3(1.0f, 1.0f, 1.0f);
        float Shininess = 32.0f;
        
        std::string DiffuseTexture;
        std::string SpecularTexture;
        std::string NormalTexture;
        
        ModelMaterial() = default;
        ModelMaterial(const std::string& name) : Name(name) {}
    };
    
    struct ModelMeshData {
        std::vector<ModelVertex> Vertices;
        std::vector<uint32_t> Indices;
        std::string MaterialName;
        std::string Name;
        
        ModelMeshData() = default;
        ModelMeshData(const std::string& name) : Name(name) {}
    };
    
    class ModelLoader {
    public:
        static bool LoadModel(const std::string& path, Model* model);
        static bool LoadOBJ(const std::string& path, Model* model);
        
    private:
        // OBJ parsing helpers
        static bool ParseOBJFile(const std::string& path, 
                                std::vector<ModelMeshData>& meshes,
                                std::unordered_map<std::string, ModelMaterial>& materials);
        
        static bool ParseMTLFile(const std::string& path,
                                std::unordered_map<std::string, ModelMaterial>& materials);
        
        static void ProcessOBJFace(const std::string& line,
                                  const std::vector<Vector3>& positions,
                                  const std::vector<Vector2>& texCoords,
                                  const std::vector<Vector3>& normals,
                                  ModelMeshData& meshData);
        
        static Vector3 ParseVector3(const std::string& line);
        static Vector2 ParseVector2(const std::string& line);
        static std::vector<std::string> SplitString(const std::string& str, char delimiter);
        static std::string TrimString(const std::string& str);
        
        // Mesh processing
        static void GenerateNormals(ModelMeshData& meshData);
        static void GenerateTangents(ModelMeshData& meshData);
        static void CalculateBoundingBox(const ModelMeshData& meshData, BoundingBox& bounds);
        
        // Engine integration
        static std::shared_ptr<Mesh> CreateEngineMesh(const ModelMeshData& meshData);
        static std::shared_ptr<Material> CreateEngineMaterial(const ModelMaterial& material, const std::string& modelDir);
    };
    
}