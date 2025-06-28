#include <SpoonEngine/SpoonEngine.h>
#include <iostream>

class TexturedCubeExampleApp : public SpoonEngine::Application {
public:
    TexturedCubeExampleApp() : Application("SpoonEngine - Textured Cube with Materials") {
        std::cout << "Textured Cube Example created" << std::endl;
    }
    
    virtual ~TexturedCubeExampleApp() {
        std::cout << "Textured Cube Example destroyed" << std::endl;
    }
    
protected:
    void OnInitialize() override {
        std::cout << "Textured Cube Example initialized" << std::endl;
        
        GetRenderer()->SetClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        
        auto world = GetWorld();
        
        // Register components
        world->RegisterComponent<SpoonEngine::Transform>();
        world->RegisterComponent<SpoonEngine::MeshRenderer>();
        world->RegisterComponent<SpoonEngine::CameraComponent>();
        
        // Register render system
        m_RenderSystem = world->RegisterSystem<SpoonEngine::RenderSystem>();
        m_RenderSystem->SetWorld(world);
        
        SpoonEngine::Signature renderSignature;
        renderSignature.set(world->GetComponentManager()->GetComponentType<SpoonEngine::Transform>());
        renderSignature.set(world->GetComponentManager()->GetComponentType<SpoonEngine::MeshRenderer>());
        world->SetSystemSignature<SpoonEngine::RenderSystem>(renderSignature);
        
        // Create advanced material shader
        std::string vertexShader = R"(
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
        )";
        
        std::string fragmentShader = R"(
            #version 330 core
            
            in vec3 v_WorldPos;
            in vec3 v_Normal;
            in vec2 v_TexCoord;
            
            uniform vec3 u_CameraPosition;
            uniform sampler2D u_MainTexture;
            uniform vec3 u_Color;
            uniform float u_Metallic;
            uniform float u_Roughness;
            uniform vec3 u_LightPosition;
            uniform vec3 u_LightColor;
            uniform float u_LightIntensity;
            
            out vec4 FragColor;
            
            void main() {
                vec3 norm = normalize(v_Normal);
                vec3 color = texture(u_MainTexture, v_TexCoord).rgb * u_Color;
                
                // Ambient
                float ambientStrength = 0.1;
                vec3 ambient = ambientStrength * u_LightColor;
                
                // Diffuse
                vec3 lightDir = normalize(u_LightPosition - v_WorldPos);
                float diff = max(dot(norm, lightDir), 0.0);
                vec3 diffuse = diff * u_LightColor * u_LightIntensity;
                
                // Specular (simplified PBR-like)
                vec3 viewDir = normalize(u_CameraPosition - v_WorldPos);
                vec3 reflectDir = reflect(-lightDir, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), mix(4.0, 64.0, 1.0 - u_Roughness));
                vec3 specular = spec * u_LightColor * u_Metallic * u_LightIntensity;
                
                vec3 result = (ambient + diffuse + specular) * color;
                FragColor = vec4(result, 1.0);
            }
        )";
        
        auto shader = SpoonEngine::Shader::Create("MaterialShader", vertexShader, fragmentShader);
        auto mesh = SpoonEngine::Mesh::CreateCube();
        
        // Create a simple checkerboard texture programmatically
        CreateCheckerboardTexture();
        
        // Create material with texture
        auto material = SpoonEngine::Material::Create(shader, "CubeMaterial");
        material->SetTexture("u_MainTexture", m_CheckerboardTexture);
        material->SetFloat3("u_Color", glm::vec3(1.0f, 1.0f, 1.0f));
        material->SetFloat("u_Metallic", 0.2f);
        material->SetFloat("u_Roughness", 0.6f);
        material->SetFloat3("u_LightPosition", glm::vec3(3.0f, 4.0f, 2.0f));
        material->SetFloat3("u_LightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        material->SetFloat("u_LightIntensity", 1.5f);
        
        // Create multiple cubes with different material properties
        for (int i = 0; i < 3; i++) {
            auto cubeEntity = world->CreateEntity();
            
            SpoonEngine::Transform transform;
            transform.Position = SpoonEngine::Vector3(i * 3.0f - 3.0f, 0.0f, 0.0f);
            transform.Scale = SpoonEngine::Vector3(1.0f, 1.0f, 1.0f);
            world->AddComponent(cubeEntity, transform);
            
            // Create a copy of the material with different properties
            auto cubeMaterial = SpoonEngine::Material::Create(shader, "CubeMaterial" + std::to_string(i));
            cubeMaterial->SetTexture("u_MainTexture", m_CheckerboardTexture);
            cubeMaterial->SetFloat3("u_Color", glm::vec3(1.0f - i * 0.3f, 0.5f + i * 0.2f, 0.3f + i * 0.3f));
            cubeMaterial->SetFloat("u_Metallic", 0.1f + i * 0.3f);
            cubeMaterial->SetFloat("u_Roughness", 0.2f + i * 0.3f);
            cubeMaterial->SetFloat3("u_LightPosition", glm::vec3(3.0f, 4.0f, 2.0f));
            cubeMaterial->SetFloat3("u_LightColor", glm::vec3(1.0f, 1.0f, 1.0f));
            cubeMaterial->SetFloat("u_LightIntensity", 1.5f);
            
            SpoonEngine::MeshRenderer renderer;
            renderer.Mesh = mesh;
            renderer.Material = cubeMaterial;
            world->AddComponent(cubeEntity, renderer);
            
            m_CubeEntities.push_back(cubeEntity);
        }
        
        // Create camera
        auto cameraEntity = world->CreateEntity();
        SpoonEngine::Transform cameraTransform;
        cameraTransform.Position = SpoonEngine::Vector3(0, 2, 8);
        world->AddComponent(cameraEntity, cameraTransform);
        
        SpoonEngine::CameraComponent cameraComp;
        cameraComp.Camera = std::make_shared<SpoonEngine::FlyCamera>();
        cameraComp.Camera->SetPerspective(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
        cameraComp.IsActive = true;
        cameraComp.Priority = 0;
        world->AddComponent(cameraEntity, cameraComp);
        
        m_CameraEntity = cameraEntity;
        m_RenderSystem->SetCamera(cameraComp.Camera);
        
        std::cout << "Created " << m_CubeEntities.size() << " textured cubes with advanced materials" << std::endl;
    }
    
    void OnUpdate(float deltaTime) override {
        if (SpoonEngine::Input::IsKeyPressed(SpoonEngine::KeyCode::Escape)) {
            Shutdown();
        }
        
        auto world = GetWorld();
        
        // Update cube rotations
        for (size_t i = 0; i < m_CubeEntities.size(); i++) {
            auto& transform = world->GetComponent<SpoonEngine::Transform>(m_CubeEntities[i]);
            transform.Rotation.y += deltaTime * (30.0f + i * 15.0f);
            transform.Rotation.x += deltaTime * (20.0f + i * 10.0f);
        }
        
        // Handle camera movement
        auto& cameraComp = world->GetComponent<SpoonEngine::CameraComponent>(m_CameraEntity);
        auto flyCamera = std::dynamic_pointer_cast<SpoonEngine::FlyCamera>(cameraComp.Camera);
        
        if (flyCamera) {
            if (SpoonEngine::Input::IsKeyPressed(SpoonEngine::KeyCode::W)) {
                flyCamera->ProcessKeyboard(static_cast<int>(SpoonEngine::KeyCode::W), deltaTime);
            }
            if (SpoonEngine::Input::IsKeyPressed(SpoonEngine::KeyCode::S)) {
                flyCamera->ProcessKeyboard(static_cast<int>(SpoonEngine::KeyCode::S), deltaTime);
            }
            if (SpoonEngine::Input::IsKeyPressed(SpoonEngine::KeyCode::A)) {
                flyCamera->ProcessKeyboard(static_cast<int>(SpoonEngine::KeyCode::A), deltaTime);
            }
            if (SpoonEngine::Input::IsKeyPressed(SpoonEngine::KeyCode::D)) {
                flyCamera->ProcessKeyboard(static_cast<int>(SpoonEngine::KeyCode::D), deltaTime);
            }
            if (SpoonEngine::Input::IsKeyPressed(SpoonEngine::KeyCode::Q)) {
                flyCamera->ProcessKeyboard(static_cast<int>(SpoonEngine::KeyCode::Q), deltaTime);
            }
            if (SpoonEngine::Input::IsKeyPressed(SpoonEngine::KeyCode::E)) {
                flyCamera->ProcessKeyboard(static_cast<int>(SpoonEngine::KeyCode::E), deltaTime);
            }
        }
    }
    
    void OnRender() override {
        // Rendering handled by render system
    }
    
    void OnShutdown() override {
        std::cout << "Textured Cube Example shutdown" << std::endl;
    }
    
private:
    void CreateCheckerboardTexture() {
        // Create a simple 8x8 checkerboard texture
        const int size = 8;
        const int channels = 3;
        unsigned char data[size * size * channels];
        
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                int index = (y * size + x) * channels;
                bool isWhite = ((x + y) % 2) == 0;
                
                if (isWhite) {
                    data[index] = 255;     // R
                    data[index + 1] = 255; // G
                    data[index + 2] = 255; // B
                } else {
                    data[index] = 64;      // R
                    data[index + 1] = 64;  // G
                    data[index + 2] = 64;  // B
                }
            }
        }
        
        SpoonEngine::TextureSpecification spec;
        spec.Width = size;
        spec.Height = size;
        spec.Format = SpoonEngine::TextureFormat::RGB8;
        spec.GenerateMips = true;
        
        m_CheckerboardTexture = SpoonEngine::Texture2D::Create(spec);
        m_CheckerboardTexture->Load();
        m_CheckerboardTexture->SetData(data, size * size * channels);
        
        std::cout << "Created procedural checkerboard texture" << std::endl;
    }
    
    std::shared_ptr<SpoonEngine::RenderSystem> m_RenderSystem;
    std::vector<SpoonEngine::Entity> m_CubeEntities;
    SpoonEngine::Entity m_CameraEntity;
    std::shared_ptr<SpoonEngine::Texture2D> m_CheckerboardTexture;
};

SPOON_MAIN(TexturedCubeExampleApp)