#include <SpoonEngine/SpoonEngine.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CubeExampleApp : public SpoonEngine::Application {
public:
    CubeExampleApp() : Application("SpoonEngine - Professional 3D Cube Example") {
        std::cout << "Professional CubeExampleApp created" << std::endl;
    }
    
    virtual ~CubeExampleApp() {
        std::cout << "CubeExampleApp destroyed" << std::endl;
    }
    
protected:
    void OnInitialize() override {
        std::cout << "CubeExampleApp initialized" << std::endl;
        
        GetRenderer()->SetClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        
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
        
        // Register camera system
        m_CameraSystem = world->RegisterSystem<SpoonEngine::CameraSystem>();
        m_CameraSystem->SetWorld(world);
        
        SpoonEngine::Signature cameraSignature;
        cameraSignature.set(world->GetComponentManager()->GetComponentType<SpoonEngine::CameraComponent>());
        world->SetSystemSignature<SpoonEngine::CameraSystem>(cameraSignature);
        
        // Create enhanced shader with better lighting
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
            
            out vec4 FragColor;
            
            void main() {
                vec3 norm = normalize(v_Normal);
                vec3 lightColor = vec3(1.0, 1.0, 1.0);
                vec3 lightPos = vec3(5.0, 5.0, 5.0);
                vec3 objectColor = vec3(0.8, 0.4, 0.2);
                
                // Ambient
                float ambientStrength = 0.1;
                vec3 ambient = ambientStrength * lightColor;
                
                // Diffuse
                vec3 lightDir = normalize(lightPos - v_WorldPos);
                float diff = max(dot(norm, lightDir), 0.0);
                vec3 diffuse = diff * lightColor;
                
                // Specular
                float specularStrength = 0.5;
                vec3 viewDir = normalize(u_CameraPosition - v_WorldPos);
                vec3 reflectDir = reflect(-lightDir, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
                vec3 specular = specularStrength * spec * lightColor;
                
                vec3 result = (ambient + diffuse + specular) * objectColor;
                FragColor = vec4(result, 1.0);
            }
        )";
        
        auto shader = SpoonEngine::Shader::Create("EnhancedCubeShader", vertexShader, fragmentShader);
        auto mesh = SpoonEngine::Mesh::CreateCube();
        
        // Create multiple rotating cubes
        for (int i = 0; i < 3; i++) {
            auto cubeEntity = world->CreateEntity();
            
            SpoonEngine::Transform transform;
            transform.Position = SpoonEngine::Vector3(i * 2.5f - 2.5f, 0.0f, 0.0f);
            transform.Scale = SpoonEngine::Vector3(1.0f, 1.0f, 1.0f);
            world->AddComponent(cubeEntity, transform);
            
            SpoonEngine::MeshRenderer renderer;
            renderer.Mesh = mesh;
            renderer.Shader = shader;
            world->AddComponent(cubeEntity, renderer);
            
            m_CubeEntities.push_back(cubeEntity);
        }
        
        // Create camera entity
        auto cameraEntity = world->CreateEntity();
        
        SpoonEngine::Transform cameraTransform;
        cameraTransform.Position = SpoonEngine::Vector3(0, 2, 5);
        world->AddComponent(cameraEntity, cameraTransform);
        
        SpoonEngine::CameraComponent cameraComp;
        cameraComp.Camera = std::make_shared<SpoonEngine::FlyCamera>();
        cameraComp.Camera->SetPerspective(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
        cameraComp.IsActive = true;
        cameraComp.Priority = 0;
        world->AddComponent(cameraEntity, cameraComp);
        
        m_CameraEntity = cameraEntity;
        
        // Set render system camera
        m_RenderSystem->SetCamera(cameraComp.Camera);
        
        std::cout << "Created " << m_CubeEntities.size() << " cube entities with enhanced lighting" << std::endl;
    }
    
    void OnUpdate(float deltaTime) override {
        if (SpoonEngine::Input::IsKeyPressed(SpoonEngine::KeyCode::Escape)) {
            Shutdown();
        }
        
        auto world = GetWorld();
        
        // Update cube rotations
        for (size_t i = 0; i < m_CubeEntities.size(); i++) {
            auto& transform = world->GetComponent<SpoonEngine::Transform>(m_CubeEntities[i]);
            transform.Rotation.y += deltaTime * (50.0f + i * 20.0f);
            transform.Rotation.x += deltaTime * (30.0f + i * 10.0f);
        }
        
        // Handle camera movement
        auto& cameraComp = world->GetComponent<SpoonEngine::CameraComponent>(m_CameraEntity);
        auto flyCamera = std::dynamic_pointer_cast<SpoonEngine::FlyCamera>(cameraComp.Camera);
        
        if (flyCamera) {
            // WASD movement
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
        // Rendering is handled by the render system
    }
    
    void OnShutdown() override {
        std::cout << "CubeExampleApp shutdown" << std::endl;
    }
    
private:
    std::shared_ptr<SpoonEngine::RenderSystem> m_RenderSystem;
    std::shared_ptr<SpoonEngine::CameraSystem> m_CameraSystem;
    std::vector<SpoonEngine::Entity> m_CubeEntities;
    SpoonEngine::Entity m_CameraEntity;
};

SPOON_MAIN(CubeExampleApp)