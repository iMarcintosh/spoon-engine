#include "SpoonEngine/Assets/ModelLoader.h"
#include "SpoonEngine/Renderer/Shader.h"
#include "SpoonEngine/Assets/Texture.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// Use platform-specific filesystem implementation
#if __has_include(<filesystem>)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    // Fallback for older compilers
    #include <string>
    namespace fs {
        struct path {
            std::string p;
            path(const std::string& s) : p(s) {}
            std::string extension() const {
                size_t pos = p.find_last_of('.');
                return pos != std::string::npos ? p.substr(pos) : "";
            }
            path parent_path() const {
                size_t pos = p.find_last_of("/\\");
                return pos != std::string::npos ? path(p.substr(0, pos)) : path("");
            }
            std::string string() const { return p; }
            path operator/(const std::string& s) const { return path(p + "/" + s); }
        };
    }
#endif

namespace SpoonEngine {
    
    bool ModelLoader::LoadModel(const std::string& path, Model* model) {
        if (!model) {
            std::cout << "Invalid model pointer" << std::endl;
            return false;
        }
        
        // Determine file format by extension
        std::string extension = fs::path(path).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        if (extension == ".obj") {
            return LoadOBJ(path, model);
        } else {
            std::cout << "Unsupported model format: " << extension << std::endl;
            return false;
        }
    }
    
    bool ModelLoader::LoadOBJ(const std::string& path, Model* model) {
        std::vector<ModelMeshData> meshes;
        std::unordered_map<std::string, ModelMaterial> materials;
        
        if (!ParseOBJFile(path, meshes, materials)) {
            return false;
        }
        
        std::string modelDir = fs::path(path).parent_path().string();
        
        // Convert parsed data to engine objects
        for (auto& meshData : meshes) {
            // Generate normals if missing
            if (!meshData.Vertices.empty() && meshData.Vertices[0].Normal == Vector3::Zero()) {
                GenerateNormals(meshData);
            }
            
            // Generate tangents for normal mapping
            GenerateTangents(meshData);
            
            // Create engine mesh
            auto engineMesh = CreateEngineMesh(meshData);
            if (!engineMesh) {
                std::cout << "Failed to create engine mesh for: " << meshData.Name << std::endl;
                continue;
            }
            
            // Create material
            std::shared_ptr<Material> engineMaterial;
            if (!meshData.MaterialName.empty() && materials.find(meshData.MaterialName) != materials.end()) {
                engineMaterial = CreateEngineMaterial(materials[meshData.MaterialName], modelDir);
            } else {
                // Create default material
                auto defaultShader = Shader::Create("DefaultModel", 
                    // Default vertex shader
                    R"(
                        #version 330 core
                        layout (location = 0) in vec3 a_Position;
                        layout (location = 1) in vec3 a_Normal;
                        layout (location = 2) in vec2 a_TexCoord;
                        
                        uniform mat4 u_Model;
                        uniform mat4 u_View;
                        uniform mat4 u_Projection;
                        
                        out vec3 v_WorldPos;
                        out vec3 v_Normal;
                        out vec2 v_TexCoord;
                        
                        void main() {
                            v_WorldPos = vec3(u_Model * vec4(a_Position, 1.0));
                            v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
                            v_TexCoord = a_TexCoord;
                            
                            gl_Position = u_Projection * u_View * vec4(v_WorldPos, 1.0);
                        }
                    )",
                    // Default fragment shader
                    R"(
                        #version 330 core
                        
                        in vec3 v_WorldPos;
                        in vec3 v_Normal;
                        in vec2 v_TexCoord;
                        
                        uniform vec3 u_Color;
                        uniform vec3 u_LightPosition;
                        uniform vec3 u_LightColor;
                        uniform vec3 u_CameraPosition;
                        
                        out vec4 FragColor;
                        
                        void main() {
                            vec3 norm = normalize(v_Normal);
                            vec3 lightDir = normalize(u_LightPosition - v_WorldPos);
                            
                            // Ambient
                            float ambientStrength = 0.1;
                            vec3 ambient = ambientStrength * u_LightColor;
                            
                            // Diffuse
                            float diff = max(dot(norm, lightDir), 0.0);
                            vec3 diffuse = diff * u_LightColor;
                            
                            // Specular
                            vec3 viewDir = normalize(u_CameraPosition - v_WorldPos);
                            vec3 reflectDir = reflect(-lightDir, norm);
                            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
                            vec3 specular = spec * u_LightColor;
                            
                            vec3 result = (ambient + diffuse + specular) * u_Color;
                            FragColor = vec4(result, 1.0);
                        }
                    )"
                );
                
                engineMaterial = Material::Create(defaultShader, "DefaultModelMaterial");
                engineMaterial->SetFloat3("u_Color", glm::vec3(0.8f, 0.8f, 0.8f));
                engineMaterial->SetFloat3("u_LightPosition", glm::vec3(5.0f, 5.0f, 5.0f));
                engineMaterial->SetFloat3("u_LightColor", glm::vec3(1.0f, 1.0f, 1.0f));
            }
            
            // Calculate bounding box for this mesh
            BoundingBox meshBounds;
            CalculateBoundingBox(meshData, meshBounds);
            
            // Create model mesh
            ModelMesh modelMesh(engineMesh, engineMaterial, meshData.Name);
            modelMesh.Bounds = meshBounds;
            
            model->m_Meshes.push_back(modelMesh);
        }
        
        return !model->m_Meshes.empty();
    }
    
    bool ModelLoader::ParseOBJFile(const std::string& path, 
                                  std::vector<ModelMeshData>& meshes,
                                  std::unordered_map<std::string, ModelMaterial>& materials) {
        
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cout << "Failed to open OBJ file: " << path << std::endl;
            return false;
        }
        
        std::vector<Vector3> positions;
        std::vector<Vector2> texCoords;
        std::vector<Vector3> normals;
        
        ModelMeshData currentMesh("default");
        std::string currentMaterial;
        
        std::string line;
        while (std::getline(file, line)) {
            line = TrimString(line);
            
            if (line.empty() || line[0] == '#') {
                continue; // Skip empty lines and comments
            }
            
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;
            
            if (prefix == "v") {
                // Vertex position
                positions.push_back(ParseVector3(line));
            }
            else if (prefix == "vt") {
                // Texture coordinate
                texCoords.push_back(ParseVector2(line));
            }
            else if (prefix == "vn") {
                // Vertex normal
                normals.push_back(ParseVector3(line));
            }
            else if (prefix == "f") {
                // Face
                ProcessOBJFace(line, positions, texCoords, normals, currentMesh);
            }
            else if (prefix == "mtllib") {
                // Material library
                std::string mtlFile;
                iss >> mtlFile;
                std::string mtlPath = (fs::path(path).parent_path() / mtlFile).string();
                ParseMTLFile(mtlPath, materials);
            }
            else if (prefix == "usemtl") {
                // Use material - start new mesh if material changes
                std::string materialName;
                iss >> materialName;
                
                if (!currentMaterial.empty() && currentMaterial != materialName && !currentMesh.Vertices.empty()) {
                    // Save current mesh and start new one
                    currentMesh.MaterialName = currentMaterial;
                    meshes.push_back(currentMesh);
                    currentMesh = ModelMeshData("mesh_" + std::to_string(meshes.size()));
                }
                
                currentMaterial = materialName;
            }
            else if (prefix == "o" || prefix == "g") {
                // Object or group name
                std::string name;
                iss >> name;
                
                if (!currentMesh.Vertices.empty()) {
                    // Save current mesh and start new one
                    currentMesh.MaterialName = currentMaterial;
                    meshes.push_back(currentMesh);
                }
                
                currentMesh = ModelMeshData(name);
            }
        }
        
        // Add final mesh if it has data
        if (!currentMesh.Vertices.empty()) {
            currentMesh.MaterialName = currentMaterial;
            meshes.push_back(currentMesh);
        }
        
        file.close();
        
        std::cout << "Parsed OBJ file: " << path << std::endl;
        std::cout << "  Positions: " << positions.size() << std::endl;
        std::cout << "  TexCoords: " << texCoords.size() << std::endl;
        std::cout << "  Normals: " << normals.size() << std::endl;
        std::cout << "  Meshes: " << meshes.size() << std::endl;
        std::cout << "  Materials: " << materials.size() << std::endl;
        
        return !meshes.empty();
    }
    
    bool ModelLoader::ParseMTLFile(const std::string& path,
                                  std::unordered_map<std::string, ModelMaterial>& materials) {
        
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cout << "Could not open MTL file: " << path << std::endl;
            return false;
        }
        
        ModelMaterial currentMaterial;
        
        std::string line;
        while (std::getline(file, line)) {
            line = TrimString(line);
            
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;
            
            if (prefix == "newmtl") {
                // New material
                if (!currentMaterial.Name.empty()) {
                    materials[currentMaterial.Name] = currentMaterial;
                }
                
                std::string name;
                iss >> name;
                currentMaterial = ModelMaterial(name);
            }
            else if (prefix == "Ka") {
                // Ambient color
                currentMaterial.Ambient = ParseVector3(line);
            }
            else if (prefix == "Kd") {
                // Diffuse color
                currentMaterial.Diffuse = ParseVector3(line);
            }
            else if (prefix == "Ks") {
                // Specular color
                currentMaterial.Specular = ParseVector3(line);
            }
            else if (prefix == "Ns") {
                // Shininess
                iss >> currentMaterial.Shininess;
            }
            else if (prefix == "map_Kd") {
                // Diffuse texture
                std::string texPath;
                iss >> texPath;
                currentMaterial.DiffuseTexture = texPath;
            }
            else if (prefix == "map_Ks") {
                // Specular texture
                std::string texPath;
                iss >> texPath;
                currentMaterial.SpecularTexture = texPath;
            }
            else if (prefix == "map_Bump" || prefix == "bump") {
                // Normal/bump texture
                std::string texPath;
                iss >> texPath;
                currentMaterial.NormalTexture = texPath;
            }
        }
        
        // Add final material
        if (!currentMaterial.Name.empty()) {
            materials[currentMaterial.Name] = currentMaterial;
        }
        
        file.close();
        
        std::cout << "Parsed MTL file: " << path << " with " << materials.size() << " materials" << std::endl;
        
        return true;
    }
    
    void ModelLoader::ProcessOBJFace(const std::string& line,
                                    const std::vector<Vector3>& positions,
                                    const std::vector<Vector2>& texCoords,
                                    const std::vector<Vector3>& normals,
                                    ModelMeshData& meshData) {
        
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix; // Skip "f"
        
        std::vector<std::string> faceVertices;
        std::string vertex;
        while (iss >> vertex) {
            faceVertices.push_back(vertex);
        }
        
        // Convert to triangles (assuming faces are triangles or quads)
        if (faceVertices.size() >= 3) {
            auto parseVertex = [&](const std::string& vertexStr) -> ModelVertex {
                auto parts = SplitString(vertexStr, '/');
                
                ModelVertex vertex;
                
                // Position (required)
                int posIndex = std::stoi(parts[0]) - 1; // OBJ uses 1-based indexing
                if (posIndex >= 0 && posIndex < positions.size()) {
                    vertex.Position = positions[posIndex];
                }
                
                // Texture coordinate (optional)
                if (parts.size() > 1 && !parts[1].empty()) {
                    int texIndex = std::stoi(parts[1]) - 1;
                    if (texIndex >= 0 && texIndex < texCoords.size()) {
                        vertex.TexCoords = texCoords[texIndex];
                    }
                }
                
                // Normal (optional)
                if (parts.size() > 2 && !parts[2].empty()) {
                    int normalIndex = std::stoi(parts[2]) - 1;
                    if (normalIndex >= 0 && normalIndex < normals.size()) {
                        vertex.Normal = normals[normalIndex];
                    }
                }
                
                return vertex;
            };
            
            // Add triangle(s)
            uint32_t baseIndex = meshData.Vertices.size();
            
            for (size_t i = 0; i < faceVertices.size(); i++) {
                meshData.Vertices.push_back(parseVertex(faceVertices[i]));
            }
            
            // Create triangles (fan triangulation for quads+)
            for (size_t i = 1; i < faceVertices.size() - 1; i++) {
                meshData.Indices.push_back(baseIndex);
                meshData.Indices.push_back(baseIndex + i);
                meshData.Indices.push_back(baseIndex + i + 1);
            }
        }
    }
    
    Vector3 ModelLoader::ParseVector3(const std::string& line) {
        std::istringstream iss(line);
        std::string prefix;
        float x, y, z;
        
        iss >> prefix >> x >> y >> z;
        return Vector3(x, y, z);
    }
    
    Vector2 ModelLoader::ParseVector2(const std::string& line) {
        std::istringstream iss(line);
        std::string prefix;
        float x, y;
        
        iss >> prefix >> x >> y;
        return Vector2(x, y);
    }
    
    std::vector<std::string> ModelLoader::SplitString(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    std::string ModelLoader::TrimString(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    void ModelLoader::GenerateNormals(ModelMeshData& meshData) {
        // Reset all normals
        for (auto& vertex : meshData.Vertices) {
            vertex.Normal = Vector3::Zero();
        }
        
        // Calculate face normals and accumulate
        for (size_t i = 0; i < meshData.Indices.size(); i += 3) {
            uint32_t i0 = meshData.Indices[i];
            uint32_t i1 = meshData.Indices[i + 1];
            uint32_t i2 = meshData.Indices[i + 2];
            
            Vector3 v0 = meshData.Vertices[i0].Position;
            Vector3 v1 = meshData.Vertices[i1].Position;
            Vector3 v2 = meshData.Vertices[i2].Position;
            
            Vector3 edge1 = v1 - v0;
            Vector3 edge2 = v2 - v0;
            Vector3 normal = edge1.Cross(edge2).Normalize();
            
            meshData.Vertices[i0].Normal += normal;
            meshData.Vertices[i1].Normal += normal;
            meshData.Vertices[i2].Normal += normal;
        }
        
        // Normalize all vertex normals
        for (auto& vertex : meshData.Vertices) {
            vertex.Normal = vertex.Normal.Normalize();
        }
    }
    
    void ModelLoader::GenerateTangents(ModelMeshData& meshData) {
        // Reset tangents and bitangents
        for (auto& vertex : meshData.Vertices) {
            vertex.Tangent = Vector3::Zero();
            vertex.Bitangent = Vector3::Zero();
        }
        
        // Calculate tangents for each triangle
        for (size_t i = 0; i < meshData.Indices.size(); i += 3) {
            uint32_t i0 = meshData.Indices[i];
            uint32_t i1 = meshData.Indices[i + 1];
            uint32_t i2 = meshData.Indices[i + 2];
            
            auto& v0 = meshData.Vertices[i0];
            auto& v1 = meshData.Vertices[i1];
            auto& v2 = meshData.Vertices[i2];
            
            Vector3 edge1 = v1.Position - v0.Position;
            Vector3 edge2 = v2.Position - v0.Position;
            
            Vector2 deltaUV1 = v1.TexCoords - v0.TexCoords;
            Vector2 deltaUV2 = v2.TexCoords - v0.TexCoords;
            
            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            
            Vector3 tangent = Vector3(
                f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
                f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
                f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
            );
            
            Vector3 bitangent = Vector3(
                f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
                f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
                f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z)
            );
            
            v0.Tangent += tangent;
            v1.Tangent += tangent;
            v2.Tangent += tangent;
            
            v0.Bitangent += bitangent;
            v1.Bitangent += bitangent;
            v2.Bitangent += bitangent;
        }
        
        // Normalize tangents and bitangents
        for (auto& vertex : meshData.Vertices) {
            vertex.Tangent = vertex.Tangent.Normalize();
            vertex.Bitangent = vertex.Bitangent.Normalize();
        }
    }
    
    void ModelLoader::CalculateBoundingBox(const ModelMeshData& meshData, BoundingBox& bounds) {
        bounds = BoundingBox();
        
        for (const auto& vertex : meshData.Vertices) {
            bounds.Expand(vertex.Position);
        }
    }
    
    std::shared_ptr<Mesh> ModelLoader::CreateEngineMesh(const ModelMeshData& meshData) {
        // Convert ModelVertex to engine vertex format
        std::vector<Vertex> vertices;
        
        for (const auto& vertex : meshData.Vertices) {
            Vertex engineVertex;
            engineVertex.Position = vertex.Position;
            engineVertex.Normal = vertex.Normal;
            engineVertex.TexCoords = vertex.TexCoords;
            engineVertex.Tangent = vertex.Tangent;
            engineVertex.Bitangent = vertex.Bitangent;
            vertices.push_back(engineVertex);
        }
        
        // Create mesh with procedural asset ID and empty path
        static AssetID meshID = 2000; // Start mesh IDs at 2000
        auto mesh = std::make_shared<Mesh>(meshID++, "");
        
        mesh->SetVertices(vertices);
        mesh->SetIndices(meshData.Indices);
        
        return mesh;
    }
    
    std::shared_ptr<Material> ModelLoader::CreateEngineMaterial(const ModelMaterial& material, const std::string& modelDir) {
        // Create basic material shader
        auto shader = Shader::Create("ModelMaterial_" + material.Name,
            // Vertex shader
            R"(
                #version 330 core
                layout (location = 0) in vec3 a_Position;
                layout (location = 1) in vec3 a_Normal;
                layout (location = 2) in vec2 a_TexCoord;
                
                uniform mat4 u_Model;
                uniform mat4 u_View;
                uniform mat4 u_Projection;
                
                out vec3 v_WorldPos;
                out vec3 v_Normal;
                out vec2 v_TexCoord;
                
                void main() {
                    v_WorldPos = vec3(u_Model * vec4(a_Position, 1.0));
                    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
                    v_TexCoord = a_TexCoord;
                    
                    gl_Position = u_Projection * u_View * vec4(v_WorldPos, 1.0);
                }
            )",
            // Fragment shader
            R"(
                #version 330 core
                
                in vec3 v_WorldPos;
                in vec3 v_Normal;
                in vec2 v_TexCoord;
                
                uniform vec3 u_Ambient;
                uniform vec3 u_Diffuse;
                uniform vec3 u_Specular;
                uniform float u_Shininess;
                uniform sampler2D u_DiffuseTexture;
                uniform vec3 u_LightPosition;
                uniform vec3 u_LightColor;
                uniform vec3 u_CameraPosition;
                
                out vec4 FragColor;
                
                void main() {
                    vec3 norm = normalize(v_Normal);
                    vec3 lightDir = normalize(u_LightPosition - v_WorldPos);
                    
                    // Sample diffuse texture
                    vec3 texColor = texture(u_DiffuseTexture, v_TexCoord).rgb;
                    
                    // Ambient
                    vec3 ambient = u_Ambient * texColor;
                    
                    // Diffuse
                    float diff = max(dot(norm, lightDir), 0.0);
                    vec3 diffuse = diff * u_Diffuse * u_LightColor * texColor;
                    
                    // Specular
                    vec3 viewDir = normalize(u_CameraPosition - v_WorldPos);
                    vec3 reflectDir = reflect(-lightDir, norm);
                    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
                    vec3 specular = spec * u_Specular * u_LightColor;
                    
                    vec3 result = ambient + diffuse + specular;
                    FragColor = vec4(result, 1.0);
                }
            )"
        );
        
        auto engineMaterial = Material::Create(shader, material.Name);
        
        // Set material properties
        engineMaterial->SetFloat3("u_Ambient", glm::vec3(material.Ambient.x, material.Ambient.y, material.Ambient.z));
        engineMaterial->SetFloat3("u_Diffuse", glm::vec3(material.Diffuse.x, material.Diffuse.y, material.Diffuse.z));
        engineMaterial->SetFloat3("u_Specular", glm::vec3(material.Specular.x, material.Specular.y, material.Specular.z));
        engineMaterial->SetFloat("u_Shininess", material.Shininess);
        
        // Load diffuse texture if specified
        if (!material.DiffuseTexture.empty()) {
            std::string texturePath = (fs::path(modelDir) / material.DiffuseTexture).string();
            auto texture = Texture2D::Create(texturePath);
            if (texture->Load()) {
                engineMaterial->SetTexture("u_DiffuseTexture", texture);
            } else {
                std::cout << "Failed to load diffuse texture: " << texturePath << std::endl;
            }
        }
        
        // Set default lighting
        engineMaterial->SetFloat3("u_LightPosition", glm::vec3(5.0f, 5.0f, 5.0f));
        engineMaterial->SetFloat3("u_LightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        
        return engineMaterial;
    }
    
}