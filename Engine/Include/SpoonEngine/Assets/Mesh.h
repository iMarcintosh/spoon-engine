#pragma once

#include "AssetManager.h"
#include "SpoonEngine/Renderer/Buffer.h"
#include "SpoonEngine/Math/Vector3.h"
#include "SpoonEngine/Math/Vector2.h"
#include <vector>
#include <memory>

namespace SpoonEngine {
    
    struct Vertex {
        Vector3 Position;
        Vector3 Normal;
        Vector2 TexCoords;
        Vector3 Tangent;
        Vector3 Bitangent;
    };
    
    struct SubMesh {
        uint32_t BaseVertex;
        uint32_t BaseIndex;
        uint32_t IndexCount;
        uint32_t MaterialIndex;
    };
    
    class Mesh : public Asset {
    public:
        Mesh(AssetID id, const std::string& path);
        virtual ~Mesh();
        
        bool Load() override;
        
        void SetVertices(const std::vector<Vertex>& vertices);
        void SetIndices(const std::vector<uint32_t>& indices);
        void SetSubMeshes(const std::vector<SubMesh>& subMeshes);
        
        const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
        const std::vector<uint32_t>& GetIndices() const { return m_Indices; }
        const std::vector<SubMesh>& GetSubMeshes() const { return m_SubMeshes; }
        
        std::shared_ptr<VertexBuffer> GetVertexBuffer() const { return m_VertexBuffer; }
        std::shared_ptr<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }
        
        void Bind() const;
        void Unbind() const;
        
        uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_Indices.size()); }
        
        static std::shared_ptr<Mesh> Create(const std::string& path);
        static std::shared_ptr<Mesh> CreateCube();
        static std::shared_ptr<Mesh> CreateSphere(float radius = 1.0f, uint32_t segments = 32);
        static std::shared_ptr<Mesh> CreatePlane(float width = 1.0f, float height = 1.0f);
        
    private:
        void UploadToGPU();
        
        std::vector<Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;
        std::vector<SubMesh> m_SubMeshes;
        
        std::shared_ptr<VertexBuffer> m_VertexBuffer;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
        std::unique_ptr<class OpenGLVertexArray> m_VertexArray;
        
        bool m_IsLoaded = false;
    };
    
}