#include "SpoonEngine/Assets/Mesh.h"
#include "SpoonEngine/Renderer/OpenGL/OpenGLBuffer.h"
#include <iostream>

namespace SpoonEngine {
    
    Mesh::Mesh(AssetID id, const std::string& path)
        : Asset(id, path) {
    }
    
    Mesh::~Mesh() {
    }
    
    bool Mesh::Load() {
        return false;
    }
    
    void Mesh::SetVertices(const std::vector<Vertex>& vertices) {
        m_Vertices = vertices;
        UploadToGPU();
    }
    
    void Mesh::SetIndices(const std::vector<uint32_t>& indices) {
        m_Indices = indices;
        UploadToGPU();
    }
    
    void Mesh::SetSubMeshes(const std::vector<SubMesh>& subMeshes) {
        m_SubMeshes = subMeshes;
    }
    
    void Mesh::Bind() const {
        if (m_VertexArray) {
            m_VertexArray->Bind();
        }
    }
    
    void Mesh::Unbind() const {
        if (m_VertexArray) {
            m_VertexArray->Unbind();
        }
    }
    
    void Mesh::UploadToGPU() {
        m_VertexArray = std::make_unique<OpenGLVertexArray>();
        
        if (!m_Vertices.empty()) {
            m_VertexBuffer = VertexBuffer::Create(reinterpret_cast<float*>(m_Vertices.data()), 
                                                 static_cast<uint32_t>(m_Vertices.size() * sizeof(Vertex)));
            
            BufferLayout layout = {
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float3, "a_Normal" },
                { ShaderDataType::Float2, "a_TexCoord" },
                { ShaderDataType::Float3, "a_Tangent" },
                { ShaderDataType::Float3, "a_Bitangent" }
            };
            m_VertexBuffer->SetLayout(layout);
            m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        }
        
        if (!m_Indices.empty()) {
            m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), static_cast<uint32_t>(m_Indices.size()));
            m_VertexArray->SetIndexBuffer(m_IndexBuffer);
        }
        
        m_IsLoaded = true;
    }
    
    std::shared_ptr<Mesh> Mesh::Create(const std::string& path) {
        return AssetManager::GetInstance().LoadAsset<Mesh>(path);
    }
    
    std::shared_ptr<Mesh> Mesh::CreateCube() {
        auto mesh = std::make_shared<Mesh>(0, "primitive_cube");
        
        std::vector<Vertex> vertices = {
            // Front face
            {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            
            // Back face
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
            {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
            
            // Top face
            {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
            {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
            
            // Bottom face
            {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
            {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
            
            // Right face
            {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            
            // Left face
            {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}
        };
        
        std::vector<uint32_t> indices = {
            0,  1,  2,   0,  2,  3,   // front
            4,  5,  6,   4,  6,  7,   // back
            8,  9,  10,  8,  10, 11,  // top
            12, 13, 14,  12, 14, 15,  // bottom
            16, 17, 18,  16, 18, 19,  // right
            20, 21, 22,  20, 22, 23   // left
        };
        
        mesh->SetVertices(vertices);
        mesh->SetIndices(indices);
        
        return mesh;
    }
    
    std::shared_ptr<Mesh> Mesh::CreateSphere(float radius, uint32_t segments) {
        auto mesh = std::make_shared<Mesh>(0, "primitive_sphere");
        return mesh;
    }
    
    std::shared_ptr<Mesh> Mesh::CreatePlane(float width, float height) {
        auto mesh = std::make_shared<Mesh>(0, "primitive_plane");
        
        std::vector<Vertex> vertices = {
            {{-width/2, 0.0f, -height/2}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{ width/2, 0.0f, -height/2}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{ width/2, 0.0f,  height/2}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-width/2, 0.0f,  height/2}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
        };
        
        std::vector<uint32_t> indices = {
            0, 1, 2, 2, 3, 0
        };
        
        mesh->SetVertices(vertices);
        mesh->SetIndices(indices);
        
        return mesh;
    }
    
}