#include "SpoonEngine/Assets/Model.h"
#include "SpoonEngine/Assets/ModelLoader.h"
#include <iostream>
#include <algorithm>

namespace SpoonEngine {
    
    Model::Model(AssetID id, const std::string& path)
        : Asset(id, path) {
    }
    
    bool Model::Load() {
        if (m_IsLoaded) {
            return true;
        }
        
        const std::string& path = GetPath();
        if (path.empty()) {
            std::cout << "Model path is empty" << std::endl;
            return false;
        }
        
        std::cout << "Loading model: " << path << std::endl;
        
        if (!ModelLoader::LoadModel(path, this)) {
            std::cout << "Failed to load model: " << path << std::endl;
            return false;
        }
        
        // Calculate bounding box for the entire model
        CalculateBoundingBox();
        
        m_IsLoaded = true;
        std::cout << "Successfully loaded model: " << path << " with " << m_Meshes.size() << " meshes" << std::endl;
        
        return true;
    }
    
    void Model::Unload() {
        m_Meshes.clear();
        m_BoundingBox = BoundingBox();
        m_IsLoaded = false;
    }
    
    void Model::CalculateBoundingBox() {
        m_BoundingBox = BoundingBox();
        
        for (auto& modelMesh : m_Meshes) {
            if (modelMesh.MeshData) {
                // Get mesh vertices and expand bounding box
                auto& mesh = modelMesh.MeshData;
                // Note: This would require access to mesh vertex data
                // For now, we'll use the individual mesh bounds
                m_BoundingBox.Min.x = std::min(m_BoundingBox.Min.x, modelMesh.Bounds.Min.x);
                m_BoundingBox.Min.y = std::min(m_BoundingBox.Min.y, modelMesh.Bounds.Min.y);
                m_BoundingBox.Min.z = std::min(m_BoundingBox.Min.z, modelMesh.Bounds.Min.z);
                
                m_BoundingBox.Max.x = std::max(m_BoundingBox.Max.x, modelMesh.Bounds.Max.x);
                m_BoundingBox.Max.y = std::max(m_BoundingBox.Max.y, modelMesh.Bounds.Max.y);
                m_BoundingBox.Max.z = std::max(m_BoundingBox.Max.z, modelMesh.Bounds.Max.z);
            }
        }
    }
    
    void Model::GenerateNormals() {
        // This would require access to vertex data
        // Implementation would go through each mesh and generate normals
        // For now, this is handled in the ModelLoader
        std::cout << "Generating normals for model (placeholder)" << std::endl;
    }
    
    void Model::GenerateTangents() {
        // This would require access to vertex data
        // Implementation would go through each mesh and generate tangents
        // For now, this is handled in the ModelLoader
        std::cout << "Generating tangents for model (placeholder)" << std::endl;
    }
    
    std::shared_ptr<Model> Model::Create(AssetID id, const std::string& path) {
        return std::make_shared<Model>(id, path);
    }
    
    std::shared_ptr<Model> Model::Create(const std::string& path) {
        static AssetID nextID = 1000; // Start model IDs at 1000
        return Create(nextID++, path);
    }
    
}