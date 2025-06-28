#pragma once

#include "SpoonEngine/Assets/AssetManager.h"
#include "SpoonEngine/Assets/Mesh.h"
#include "SpoonEngine/Renderer/Material.h"
#include "SpoonEngine/Math/Vector3.h"
#include <vector>
#include <memory>
#include <string>

namespace SpoonEngine {
    
    struct BoundingBox {
        Vector3 Min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
        Vector3 Max = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        
        Vector3 GetCenter() const {
            return Vector3(
                (Min.x + Max.x) * 0.5f,
                (Min.y + Max.y) * 0.5f,
                (Min.z + Max.z) * 0.5f
            );
        }
        
        Vector3 GetSize() const {
            return Vector3(
                Max.x - Min.x,
                Max.y - Min.y,
                Max.z - Min.z
            );
        }
        
        void Expand(const Vector3& point) {
            if (point.x < Min.x) Min.x = point.x;
            if (point.y < Min.y) Min.y = point.y;
            if (point.z < Min.z) Min.z = point.z;
            
            if (point.x > Max.x) Max.x = point.x;
            if (point.y > Max.y) Max.y = point.y;
            if (point.z > Max.z) Max.z = point.z;
        }
    };
    
    struct ModelMesh {
        std::shared_ptr<Mesh> MeshData;
        std::shared_ptr<Material> Material;
        std::string Name;
        BoundingBox Bounds;
        
        ModelMesh() = default;
        ModelMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<class Material> material, const std::string& name = "")
            : MeshData(mesh), Material(material), Name(name) {}
    };
    
    class Model : public Asset {
    public:
        Model(AssetID id, const std::string& path);
        virtual ~Model() = default;
        
        bool Load() override;
        void Unload();
        bool IsLoaded() const { return m_IsLoaded; }
        
        // Mesh access
        const std::vector<ModelMesh>& GetMeshes() const { return m_Meshes; }
        size_t GetMeshCount() const { return m_Meshes.size(); }
        const ModelMesh& GetMesh(size_t index) const { return m_Meshes[index]; }
        
        // Bounding information
        const BoundingBox& GetBoundingBox() const { return m_BoundingBox; }
        Vector3 GetCenter() const { return m_BoundingBox.GetCenter(); }
        Vector3 GetSize() const { return m_BoundingBox.GetSize(); }
        
        // Utility functions
        void CalculateBoundingBox();
        void GenerateNormals();
        void GenerateTangents();
        
        // Factory method
        static std::shared_ptr<Model> Create(AssetID id, const std::string& path);
        static std::shared_ptr<Model> Create(const std::string& path);
        
    protected:
        std::vector<ModelMesh> m_Meshes;
        BoundingBox m_BoundingBox;
        bool m_IsLoaded = false;
        
        friend class ModelLoader;
    };
    
}