#include <SpoonEngine/SpoonEngine.h>
#include <iostream>

#ifdef SPOON_PLATFORM_MACOS
    #define GL_SILENCE_DEPRECATION
    #include <OpenGL/gl3.h>
#else
    #include <GL/glew.h>
#endif

class ShadowMappingExampleApp : public SpoonEngine::Application {
public:
    ShadowMappingExampleApp() : Application("SpoonEngine - Shadow Mapping Demo") {
        std::cout << "Shadow Mapping Example created" << std::endl;
    }
    
    virtual ~ShadowMappingExampleApp() {
        std::cout << "Shadow Mapping Example destroyed" << std::endl;
    }
    
protected:
    void OnInitialize() override {
        std::cout << "Shadow Mapping Example initialized" << std::endl;
        
        GetRenderer()->SetClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        
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
        
        // Create shadow mapping framebuffer
        CreateShadowMapFramebuffer();
        
        // Create shadow mapping shader
        CreateShadowMappingShaders();
        
        // Create scene objects
        CreateScene();
        
        // Setup camera
        SetupCamera();
        
        std::cout << "Shadow mapping demo initialized with " << m_SceneEntities.size() << " objects" << std::endl;
    }
    
private:
    void CreateShadowMapFramebuffer() {
        SpoonEngine::FramebufferSpecification shadowMapSpec;
        shadowMapSpec.Width = SHADOW_MAP_SIZE;
        shadowMapSpec.Height = SHADOW_MAP_SIZE;
        shadowMapSpec.Attachments = { SpoonEngine::FramebufferTextureFormat::DEPTH24STENCIL8 };
        shadowMapSpec.SwapChainTarget = false;
        
        m_ShadowMapFramebuffer = SpoonEngine::Framebuffer::Create(shadowMapSpec);
        
        std::cout << "Created shadow map framebuffer (" << SHADOW_MAP_SIZE << "x" << SHADOW_MAP_SIZE << ")" << std::endl;
    }
    
    void CreateShadowMappingShaders() {
        // Shadow map generation shader (depth-only pass)
        std::string shadowVertexShader = R"(
            #version 330 core
            layout (location = 0) in vec3 a_Position;
            
            uniform mat4 u_LightSpaceMatrix;
            uniform mat4 u_Model;
            
            void main() {
                gl_Position = u_LightSpaceMatrix * u_Model * vec4(a_Position, 1.0);
            }
        )";
        
        std::string shadowFragmentShader = R"(
            #version 330 core
            
            void main() {
                // gl_FragDepth is written automatically
            }
        )";
        
        m_ShadowMapShader = SpoonEngine::Shader::Create("ShadowMap", shadowVertexShader, shadowFragmentShader);
        
        // Main rendering shader with shadow mapping
        std::string mainVertexShader = R"(
            #version 330 core
            layout (location = 0) in vec3 a_Position;
            layout (location = 1) in vec3 a_Normal;
            layout (location = 2) in vec2 a_TexCoord;
            
            uniform mat4 u_Model;
            uniform mat4 u_View;
            uniform mat4 u_Projection;
            uniform mat4 u_LightSpaceMatrix;
            
            out vec3 v_WorldPos;
            out vec3 v_Normal;
            out vec2 v_TexCoord;
            out vec4 v_FragPosLightSpace;
            
            void main() {
                v_WorldPos = vec3(u_Model * vec4(a_Position, 1.0));
                v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
                v_TexCoord = a_TexCoord;
                v_FragPosLightSpace = u_LightSpaceMatrix * vec4(v_WorldPos, 1.0);
                
                gl_Position = u_Projection * u_View * vec4(v_WorldPos, 1.0);
            }
        )";
        
        std::string mainFragmentShader = R"(
            #version 330 core
            
            in vec3 v_WorldPos;
            in vec3 v_Normal;
            in vec2 v_TexCoord;
            in vec4 v_FragPosLightSpace;
            
            uniform vec3 u_CameraPosition;
            uniform sampler2D u_MainTexture;
            uniform sampler2D u_ShadowMap;
            uniform vec3 u_Color;
            uniform vec3 u_LightPosition;
            uniform vec3 u_LightColor;
            uniform float u_LightIntensity;
            
            out vec4 FragColor;
            
            float ShadowCalculation(vec4 fragPosLightSpace) {
                // Perform perspective divide
                vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
                
                // Transform to [0,1] range
                projCoords = projCoords * 0.5 + 0.5;
                
                // Get closest depth value from light's perspective
                float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
                
                // Get depth of current fragment from light's perspective
                float currentDepth = projCoords.z;
                
                // Check whether current frag pos is in shadow
                float bias = 0.005;
                float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
                
                // PCF (Percentage Closer Filtering) for softer shadows
                shadow = 0.0;
                vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
                for(int x = -1; x <= 1; ++x) {
                    for(int y = -1; y <= 1; ++y) {
                        float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
                    }
                }
                shadow /= 9.0;
                
                return shadow;
            }
            
            void main() {
                vec3 norm = normalize(v_Normal);
                vec3 color = texture(u_MainTexture, v_TexCoord).rgb * u_Color;
                
                // Ambient
                float ambientStrength = 0.15;
                vec3 ambient = ambientStrength * u_LightColor;
                
                // Diffuse
                vec3 lightDir = normalize(u_LightPosition - v_WorldPos);
                float diff = max(dot(norm, lightDir), 0.0);
                vec3 diffuse = diff * u_LightColor * u_LightIntensity;
                
                // Specular
                vec3 viewDir = normalize(u_CameraPosition - v_WorldPos);
                vec3 reflectDir = reflect(-lightDir, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
                vec3 specular = spec * u_LightColor * u_LightIntensity * 0.5;
                
                // Calculate shadow
                float shadow = ShadowCalculation(v_FragPosLightSpace);
                
                // Combine lighting with shadow
                vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);
                vec3 result = lighting * color;
                
                FragColor = vec4(result, 1.0);
            }
        )";
        
        m_MainShader = SpoonEngine::Shader::Create("ShadowMappingMain", mainVertexShader, mainFragmentShader);
        
        std::cout << "Created shadow mapping shaders" << std::endl;
    }
    
    void CreateScene() {
        auto world = GetWorld();
        auto mesh = SpoonEngine::Mesh::CreateCube();
        
        // Create checkerboard texture
        CreateCheckerboardTexture();
        
        // Create material
        auto material = SpoonEngine::Material::Create(m_MainShader, "ShadowMaterial");
        material->SetTexture("u_MainTexture", m_CheckerboardTexture);
        material->SetFloat3("u_Color", glm::vec3(1.0f, 1.0f, 1.0f));
        material->SetFloat3("u_LightPosition", glm::vec3(m_LightPosition.x, m_LightPosition.y, m_LightPosition.z));
        material->SetFloat3("u_LightColor", glm::vec3(1.0f, 1.0f, 0.9f));
        material->SetFloat("u_LightIntensity", 1.0f);
        
        // Ground plane (larger cube, scaled and positioned)
        auto groundEntity = world->CreateEntity();
        SpoonEngine::Transform groundTransform;
        groundTransform.Position = SpoonEngine::Vector3(0.0f, -1.0f, 0.0f);
        groundTransform.Scale = SpoonEngine::Vector3(10.0f, 0.1f, 10.0f);
        world->AddComponent(groundEntity, groundTransform);
        
        auto groundMaterial = SpoonEngine::Material::Create(m_MainShader, "GroundMaterial");
        groundMaterial->SetTexture("u_MainTexture", m_CheckerboardTexture);
        groundMaterial->SetFloat3("u_Color", glm::vec3(0.8f, 0.8f, 0.8f));
        groundMaterial->SetFloat3("u_LightPosition", glm::vec3(m_LightPosition.x, m_LightPosition.y, m_LightPosition.z));
        groundMaterial->SetFloat3("u_LightColor", glm::vec3(1.0f, 1.0f, 0.9f));
        groundMaterial->SetFloat("u_LightIntensity", 1.0f);
        
        SpoonEngine::MeshRenderer groundRenderer;
        groundRenderer.Mesh = mesh;
        groundRenderer.Material = groundMaterial;
        world->AddComponent(groundEntity, groundRenderer);
        m_SceneEntities.push_back(groundEntity);
        
        // Create several cubes at different positions
        std::vector<SpoonEngine::Vector3> cubePositions = {
            SpoonEngine::Vector3(-2.0f, 0.5f, -2.0f),
            SpoonEngine::Vector3(2.0f, 0.5f, -1.0f),
            SpoonEngine::Vector3(-1.0f, 0.5f, 2.0f),
            SpoonEngine::Vector3(3.0f, 1.5f, 1.0f),
            SpoonEngine::Vector3(-3.0f, 0.5f, 0.0f)
        };
        
        for (size_t i = 0; i < cubePositions.size(); i++) {
            auto cubeEntity = world->CreateEntity();
            
            SpoonEngine::Transform transform;
            transform.Position = cubePositions[i];
            transform.Scale = SpoonEngine::Vector3(1.0f, 1.0f, 1.0f);
            world->AddComponent(cubeEntity, transform);
            
            auto cubeMaterial = SpoonEngine::Material::Create(m_MainShader, "CubeMaterial" + std::to_string(i));
            cubeMaterial->SetTexture("u_MainTexture", m_CheckerboardTexture);
            cubeMaterial->SetFloat3("u_Color", glm::vec3(
                0.6f + (i * 0.15f),
                0.4f + (i * 0.1f), 
                0.8f - (i * 0.1f)
            ));
            cubeMaterial->SetFloat3("u_LightPosition", glm::vec3(m_LightPosition.x, m_LightPosition.y, m_LightPosition.z));
            cubeMaterial->SetFloat3("u_LightColor", glm::vec3(1.0f, 1.0f, 0.9f));
            cubeMaterial->SetFloat("u_LightIntensity", 1.0f);
            
            SpoonEngine::MeshRenderer renderer;
            renderer.Mesh = mesh;
            renderer.Material = cubeMaterial;
            world->AddComponent(cubeEntity, renderer);
            
            m_SceneEntities.push_back(cubeEntity);
        }
    }
    
    void SetupCamera() {
        auto world = GetWorld();
        
        // Create camera
        auto cameraEntity = world->CreateEntity();
        SpoonEngine::Transform cameraTransform;
        cameraTransform.Position = SpoonEngine::Vector3(6, 4, 6);
        world->AddComponent(cameraEntity, cameraTransform);
        
        SpoonEngine::CameraComponent cameraComp;
        cameraComp.Camera = std::make_shared<SpoonEngine::FlyCamera>();
        cameraComp.Camera->SetPerspective(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
        cameraComp.IsActive = true;
        cameraComp.Priority = 0;
        world->AddComponent(cameraEntity, cameraComp);
        
        m_CameraEntity = cameraEntity;
        m_RenderSystem->SetCamera(cameraComp.Camera);
    }
    
    void CreateCheckerboardTexture() {
        const int size = 16;
        const int channels = 3;
        unsigned char data[size * size * channels];
        
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                int index = (y * size + x) * channels;
                bool isWhite = ((x + y) % 2) == 0;
                
                if (isWhite) {
                    data[index] = 220;     // R
                    data[index + 1] = 220; // G
                    data[index + 2] = 220; // B
                } else {
                    data[index] = 50;      // R
                    data[index + 1] = 50;  // G
                    data[index + 2] = 50;  // B
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
        
        std::cout << "Created checkerboard texture for shadow mapping demo" << std::endl;
    }
    
    glm::mat4 CalculateLightSpaceMatrix() {
        float near_plane = 1.0f, far_plane = 20.0f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        
        glm::vec3 lightPos(m_LightPosition.x, m_LightPosition.y, m_LightPosition.z);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        
        return lightProjection * lightView;
    }
    
    void RenderShadowMap() {
        glm::mat4 lightSpaceMatrix = CalculateLightSpaceMatrix();
        
        // Render to shadow map
        m_ShadowMapFramebuffer->Bind();
        GetRenderer()->Clear();
        
        // Configure viewport
        glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
        
        auto world = GetWorld();
        
        // Render all objects to shadow map
        for (auto entity : m_SceneEntities) {
            if (world->HasComponent<SpoonEngine::Transform>(entity) && 
                world->HasComponent<SpoonEngine::MeshRenderer>(entity)) {
                
                auto& transform = world->GetComponent<SpoonEngine::Transform>(entity);
                auto& meshRenderer = world->GetComponent<SpoonEngine::MeshRenderer>(entity);
                
                if (meshRenderer.Mesh) {
                    glm::mat4 model = transform.GetTransformMatrix();
                    
                    m_ShadowMapShader->Bind();
                    m_ShadowMapShader->SetMat4("u_LightSpaceMatrix", lightSpaceMatrix);
                    m_ShadowMapShader->SetMat4("u_Model", model);
                    
                    meshRenderer.Mesh->Bind();
                    GetRenderer()->DrawIndexed(meshRenderer.Mesh->GetIndexCount());
                    meshRenderer.Mesh->Unbind();
                    
                    m_ShadowMapShader->Unbind();
                }
            }
        }
        
        m_ShadowMapFramebuffer->Unbind();
        
        // Store light space matrix for main rendering pass
        m_LightSpaceMatrix = lightSpaceMatrix;
    }
    
    void RenderMainPass() {
        // Bind default framebuffer and configure viewport
        glViewport(0, 0, 1280, 720); // Should get actual window size
        
        auto world = GetWorld();
        auto& cameraComp = world->GetComponent<SpoonEngine::CameraComponent>(m_CameraEntity);
        
        // Update all materials with shadow map and light space matrix
        for (auto entity : m_SceneEntities) {
            if (world->HasComponent<SpoonEngine::MeshRenderer>(entity)) {
                auto& meshRenderer = world->GetComponent<SpoonEngine::MeshRenderer>(entity);
                if (meshRenderer.Material) {
                    // Bind shadow map texture
                    auto shadowMapTexture = m_ShadowMapFramebuffer->GetDepthAttachment();
                    if (shadowMapTexture) {
                        meshRenderer.Material->SetTexture("u_ShadowMap", shadowMapTexture);
                    }
                    meshRenderer.Material->SetMat4("u_LightSpaceMatrix", m_LightSpaceMatrix);
                    meshRenderer.Material->SetFloat3("u_CameraPosition", 
                        glm::vec3(cameraComp.Camera->GetPosition().x, 
                                 cameraComp.Camera->GetPosition().y, 
                                 cameraComp.Camera->GetPosition().z));
                }
            }
        }
        
        // Render using the render system
        m_RenderSystem->Render(GetRenderer());
    }
    
public:
    void OnUpdate(float deltaTime) override {
        if (SpoonEngine::Input::IsKeyPressed(SpoonEngine::KeyCode::Escape)) {
            Shutdown();
        }
        
        auto world = GetWorld();
        
        // Animate light position
        m_LightAngle += deltaTime * 30.0f;
        m_LightPosition = SpoonEngine::Vector3(
            cos(glm::radians(m_LightAngle)) * 8.0f,
            6.0f,
            sin(glm::radians(m_LightAngle)) * 8.0f
        );
        
        // Update light position in all materials
        for (auto entity : m_SceneEntities) {
            if (world->HasComponent<SpoonEngine::MeshRenderer>(entity)) {
                auto& meshRenderer = world->GetComponent<SpoonEngine::MeshRenderer>(entity);
                if (meshRenderer.Material) {
                    meshRenderer.Material->SetFloat3("u_LightPosition", glm::vec3(m_LightPosition.x, m_LightPosition.y, m_LightPosition.z));
                }
            }
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
        // Two-pass rendering: shadow map generation + main scene
        RenderShadowMap();
        RenderMainPass();
    }
    
    void OnShutdown() override {
        std::cout << "Shadow Mapping Example shutdown" << std::endl;
    }
    
private:
    static const uint32_t SHADOW_MAP_SIZE = 2048;
    
    std::shared_ptr<SpoonEngine::RenderSystem> m_RenderSystem;
    std::vector<SpoonEngine::Entity> m_SceneEntities;
    SpoonEngine::Entity m_CameraEntity;
    std::shared_ptr<SpoonEngine::Texture2D> m_CheckerboardTexture;
    
    // Shadow mapping resources
    std::shared_ptr<SpoonEngine::Framebuffer> m_ShadowMapFramebuffer;
    std::shared_ptr<SpoonEngine::Shader> m_ShadowMapShader;
    std::shared_ptr<SpoonEngine::Shader> m_MainShader;
    
    // Lighting
    SpoonEngine::Vector3 m_LightPosition = SpoonEngine::Vector3(5.0f, 6.0f, 2.0f);
    float m_LightAngle = 0.0f;
    glm::mat4 m_LightSpaceMatrix;
};

SPOON_MAIN(ShadowMappingExampleApp)