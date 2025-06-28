#include <SpoonEngine/SpoonEngine.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

class SpoonEditorApp : public SpoonEngine::Application {
public:
    SpoonEditorApp() : Application("SpoonEngine Editor") {
        std::cout << "SpoonEngine Editor starting..." << std::endl;
    }
    
    virtual ~SpoonEditorApp() {
        std::cout << "SpoonEngine Editor shutting down..." << std::endl;
    }
    
protected:
    void OnInitialize() override {
        std::cout << "Editor initialized" << std::endl;
        
        GetRenderer()->SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        
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
        
        // Initialize ImGui
        InitializeImGui();
        
        // Create viewport framebuffer
        CreateViewportFramebuffer();
        
        // Create default scene content for the editor
        CreateDefaultScene();
        
        std::cout << "Basic Editor setup complete" << std::endl;
    }
    
    void OnUpdate(float deltaTime) override {
        if (SpoonEngine::Input::IsKeyPressed(SpoonEngine::KeyCode::Escape)) {
            Shutdown();
        }
        
        // Basic camera controls for editor
        auto world = GetWorld();
        if (m_CameraEntity.IsValid()) {
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
        
        // Update materials with camera position
        UpdateMaterialsWithCameraPosition();
    }
    
    void OnRender() override {
        // Render scene to framebuffer safely
        if (m_ViewportFramebuffer && m_RenderSystem) {
            try {
                m_ViewportFramebuffer->Bind();
                GetRenderer()->SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                GetRenderer()->Clear();
                
                // Render the scene to the framebuffer
                m_RenderSystem->Render(GetRenderer());
                
                m_ViewportFramebuffer->Unbind();
            } catch (...) {
                // If framebuffer rendering fails, unbind and continue
                m_ViewportFramebuffer->Unbind();
            }
        }
        
        // Reset to main framebuffer and clear
        GetRenderer()->SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        GetRenderer()->Clear();
        
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // Render ImGui editor interface
        try {
            RenderEditorUI();
        } catch (...) {
            // If UI rendering fails, continue with ImGui cleanup
        }
        
        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    
    void OnShutdown() override {
        // Cleanup ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        std::cout << "Basic Editor shutdown" << std::endl;
    }
    
private:
    void InitializeImGui() {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        
        // Get GLFW window - we need to get this from the engine
        GLFWwindow* window = static_cast<GLFWwindow*>(GetWindow()->GetNativeWindow());
        
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        
        std::cout << "ImGui initialized for editor" << std::endl;
    }
    
    void RenderEditorUI() {
        // Simple menu bar at the top
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Scene")) {
                    CreateNewScene();
                }
                if (ImGui::MenuItem("Open Scene")) {
                    LoadScene();
                }
                if (ImGui::MenuItem("Save Scene")) {
                    SaveScene();
                }
                if (ImGui::MenuItem("Save Scene As...")) {
                    SaveSceneAs();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit")) {
                    Shutdown();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Scene Hierarchy", nullptr, &m_ShowSceneHierarchy);
                ImGui::MenuItem("Properties", nullptr, &m_ShowProperties);
                ImGui::MenuItem("Viewport", nullptr, &m_ShowViewport);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        
        // Render editor panels as separate windows
        if (m_ShowSceneHierarchy) {
            RenderSceneHierarchy();
        }
        
        if (m_ShowProperties) {
            RenderProperties();
        }
        
        if (m_ShowViewport) {
            RenderViewport();
        }
        
        // Render dialogs every frame
        RenderSaveAsDialog();
        RenderOpenDialog();
    }
    
    void RenderSceneHierarchy() {
        if (!ImGui::Begin("Scene Hierarchy", &m_ShowSceneHierarchy)) {
            ImGui::End();
            return;
        }
        
        auto world = GetWorld();
        if (!world) {
            ImGui::Text("World not available");
            ImGui::End();
            return;
        }
        
        ImGui::Text("Scene Objects (%zu)", m_SceneEntities.size());
        ImGui::Separator();
        
        for (size_t i = 0; i < m_SceneEntities.size(); i++) {
            auto entity = m_SceneEntities[i];
            if (!entity.IsValid()) continue;
            
            std::string entityName = "Entity " + std::to_string(entity.GetID());
            
            bool isSelected = (m_SelectedEntity.IsValid() && m_SelectedEntity.GetID() == entity.GetID());
            if (ImGui::Selectable(entityName.c_str(), isSelected)) {
                m_SelectedEntity = entity;
            }
            
            // Show transform info safely
            if (world->HasComponent<SpoonEngine::Transform>(entity)) {
                auto& transform = world->GetComponent<SpoonEngine::Transform>(entity);
                ImGui::SameLine();
                ImGui::TextDisabled("(%.1f, %.1f, %.1f)", transform.Position.x, transform.Position.y, transform.Position.z);
            }
        }
        
        // Camera entity
        if (m_CameraEntity.IsValid()) {
            std::string cameraName = "Camera";
            bool isSelected = (m_SelectedEntity.IsValid() && m_SelectedEntity.GetID() == m_CameraEntity.GetID());
            if (ImGui::Selectable(cameraName.c_str(), isSelected)) {
                m_SelectedEntity = m_CameraEntity;
            }
            ImGui::SameLine();
            ImGui::TextDisabled("(Camera)");
        }
        
        ImGui::End();
    }
    
    void RenderProperties() {
        if (!ImGui::Begin("Properties", &m_ShowProperties)) {
            ImGui::End();
            return;
        }
        
        if (!m_SelectedEntity.IsValid()) {
            ImGui::Text("No entity selected");
            ImGui::End();
            return;
        }
        
        auto world = GetWorld();
        if (!world) {
            ImGui::Text("World not available");
            ImGui::End();
            return;
        }
        
        ImGui::Text("Entity ID: %u", m_SelectedEntity.GetID());
        ImGui::Separator();
        
        // Transform component
        if (world->HasComponent<SpoonEngine::Transform>(m_SelectedEntity)) {
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                auto& transform = world->GetComponent<SpoonEngine::Transform>(m_SelectedEntity);
                
                ImGui::DragFloat3("Position", &transform.Position.x, 0.1f);
                ImGui::DragFloat3("Rotation", &transform.Rotation.x, 1.0f);
                ImGui::DragFloat3("Scale", &transform.Scale.x, 0.1f, 0.1f, 10.0f);
            }
        }
        
        // MeshRenderer component
        if (world->HasComponent<SpoonEngine::MeshRenderer>(m_SelectedEntity)) {
            if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
                auto& meshRenderer = world->GetComponent<SpoonEngine::MeshRenderer>(m_SelectedEntity);
                
                ImGui::Text("Mesh: %s", meshRenderer.Mesh ? "Loaded" : "None");
                ImGui::Text("Material: %s", meshRenderer.Material ? "Assigned" : "None");
            }
        }
        
        // Camera component
        if (world->HasComponent<SpoonEngine::CameraComponent>(m_SelectedEntity)) {
            if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
                auto& cameraComp = world->GetComponent<SpoonEngine::CameraComponent>(m_SelectedEntity);
                
                ImGui::Checkbox("Active", &cameraComp.IsActive);
                ImGui::DragInt("Priority", &cameraComp.Priority);
                
                if (cameraComp.Camera) {
                    auto pos = cameraComp.Camera->GetPosition();
                    ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
                }
            }
        }
        
        ImGui::End();
    }
    
    void RenderViewport() {
        if (!ImGui::Begin("Viewport", &m_ShowViewport)) {
            ImGui::End();
            return;
        }
        
        // Display framebuffer texture
        if (m_ViewportFramebuffer) {
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            
            // Safety check for valid panel size
            if (viewportPanelSize.x <= 0.0f || viewportPanelSize.y <= 0.0f) {
                ImGui::Text("Viewport too small");
                ImGui::End();
                return;
            }
            
            // Maintain aspect ratio of 16:9
            float aspectRatio = 16.0f / 9.0f;
            float panelWidth = viewportPanelSize.x;
            float panelHeight = panelWidth / aspectRatio;
            
            if (panelHeight > viewportPanelSize.y) {
                panelHeight = viewportPanelSize.y;
                panelWidth = panelHeight * aspectRatio;
            }
            
            // Ensure minimum viable size
            panelWidth = std::max(panelWidth, 128.0f);
            panelHeight = std::max(panelHeight, 72.0f);
            
            // Only resize if dimensions changed significantly (reduce flickering)
            uint32_t fbWidth = static_cast<uint32_t>(panelWidth);
            uint32_t fbHeight = static_cast<uint32_t>(panelHeight);
            
            float sizeDiffX = std::abs(m_ViewportSize.x - static_cast<float>(fbWidth));
            float sizeDiffY = std::abs(m_ViewportSize.y - static_cast<float>(fbHeight));
            
            if (sizeDiffX > 5.0f || sizeDiffY > 5.0f) {
                try {
                    m_ViewportFramebuffer->Resize(fbWidth, fbHeight);
                    m_ViewportSize = ImVec2(static_cast<float>(fbWidth), static_cast<float>(fbHeight));
                    
                    // Update camera aspect ratio safely
                    if (m_CameraEntity.IsValid() && fbHeight > 0) {
                        auto world = GetWorld();
                        if (world->HasComponent<SpoonEngine::CameraComponent>(m_CameraEntity)) {
                            auto& cameraComp = world->GetComponent<SpoonEngine::CameraComponent>(m_CameraEntity);
                            if (cameraComp.Camera) {
                                float newAspectRatio = static_cast<float>(fbWidth) / static_cast<float>(fbHeight);
                                if (newAspectRatio > 0.2f && newAspectRatio < 5.0f) {
                                    cameraComp.Camera->SetPerspective(45.0f, newAspectRatio, 0.1f, 100.0f);
                                }
                            }
                        }
                    }
                } catch (...) {
                    // Handle framebuffer resize error gracefully
                    ImGui::Text("Viewport resize error");
                    ImGui::End();
                    return;
                }
            }
            
            uint32_t textureID = m_ViewportFramebuffer->GetColorAttachmentRendererID();
            if (textureID > 0) {
                ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2(panelWidth, panelHeight), ImVec2(0, 1), ImVec2(1, 0));
            } else {
                ImGui::Text("Viewport not ready");
            }
        }
        
        ImGui::Separator();
        
        ImGui::Text("Viewport Controls:");
        ImGui::Text("WASD - Move camera");
        ImGui::Text("Q/E - Up/Down");
        ImGui::Text("ESC - Exit editor");
        
        if (m_CameraEntity.IsValid()) {
            auto world = GetWorld();
            if (world->HasComponent<SpoonEngine::CameraComponent>(m_CameraEntity)) {
                auto& cameraComp = world->GetComponent<SpoonEngine::CameraComponent>(m_CameraEntity);
                if (cameraComp.Camera) {
                    auto pos = cameraComp.Camera->GetPosition();
                    ImGui::Text("Camera Position:");
                    ImGui::Text("X: %.2f, Y: %.2f, Z: %.2f", pos.x, pos.y, pos.z);
                }
            }
        }
        
        ImGui::End();
    }
    
    void CreateDefaultScene() {
        auto world = GetWorld();
        
        // Create basic material shader
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
        )";
        
        auto shader = SpoonEngine::Shader::Create("EditorBasic", vertexShader, fragmentShader);
        auto mesh = SpoonEngine::Mesh::CreateCube();
        
        // Create some sample objects for the editor scene
        std::vector<SpoonEngine::Vector3> positions = {
            SpoonEngine::Vector3(0, 0, 0),
            SpoonEngine::Vector3(3, 0, 0),
            SpoonEngine::Vector3(-3, 0, 0),
            SpoonEngine::Vector3(0, 0, 3),
            SpoonEngine::Vector3(0, 0, -3)
        };
        
        std::vector<SpoonEngine::Vector3> colors = {
            SpoonEngine::Vector3(0.8f, 0.3f, 0.3f),  // Red
            SpoonEngine::Vector3(0.3f, 0.8f, 0.3f),  // Green
            SpoonEngine::Vector3(0.3f, 0.3f, 0.8f),  // Blue
            SpoonEngine::Vector3(0.8f, 0.8f, 0.3f),  // Yellow
            SpoonEngine::Vector3(0.8f, 0.3f, 0.8f)   // Magenta
        };
        
        for (size_t i = 0; i < positions.size(); i++) {
            auto entity = world->CreateEntity();
            
            SpoonEngine::Transform transform;
            transform.Position = positions[i];
            transform.Scale = SpoonEngine::Vector3(1.0f, 1.0f, 1.0f);
            world->AddComponent(entity, transform);
            
            auto material = SpoonEngine::Material::Create(shader, "EditorObject" + std::to_string(i));
            material->SetFloat3("u_Color", glm::vec3(colors[i].x, colors[i].y, colors[i].z));
            material->SetFloat3("u_LightPosition", glm::vec3(5.0f, 5.0f, 5.0f));
            material->SetFloat3("u_LightColor", glm::vec3(1.0f, 1.0f, 1.0f));
            
            SpoonEngine::MeshRenderer renderer;
            renderer.Mesh = mesh;
            renderer.Material = material;
            world->AddComponent(entity, renderer);
            
            m_SceneEntities.push_back(entity);
        }
        
        // Create camera with better positioning to see the objects
        auto cameraEntity = world->CreateEntity();
        SpoonEngine::Transform cameraTransform;
        cameraTransform.Position = SpoonEngine::Vector3(10, 8, 10);
        world->AddComponent(cameraEntity, cameraTransform);
        
        SpoonEngine::CameraComponent cameraComp;
        auto flyCamera = std::make_shared<SpoonEngine::FlyCamera>();
        
        // Set camera position and make it look toward the scene center
        flyCamera->SetPosition(SpoonEngine::Vector3(10, 8, 10));
        
        // Calculate initial yaw/pitch to look at origin from (10, 8, 10)
        SpoonEngine::Vector3 target(0, 0, 0);  // Scene center
        SpoonEngine::Vector3 position(10, 8, 10);
        SpoonEngine::Vector3 direction = target - position;  // Direction to look
        
        // Calculate yaw (rotation around Y axis) and pitch (rotation around X axis)
        float yaw = glm::degrees(atan2(direction.z, direction.x));
        float pitch = glm::degrees(atan2(-direction.y, sqrt(direction.x * direction.x + direction.z * direction.z)));
        
        // Set initial mouse look values through mouse movement simulation
        flyCamera->ProcessMouseMovement(yaw, pitch);
        
        flyCamera->SetPerspective(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
        cameraComp.Camera = flyCamera;
        cameraComp.IsActive = true;
        cameraComp.Priority = 0;
        world->AddComponent(cameraEntity, cameraComp);
        
        m_CameraEntity = cameraEntity;
        m_RenderSystem->SetCamera(cameraComp.Camera);
        
        std::cout << "Created editor scene with " << m_SceneEntities.size() << " objects" << std::endl;
    }
    
    void CreateViewportFramebuffer() {
        SpoonEngine::FramebufferSpecification fbSpec;
        fbSpec.Attachments = { SpoonEngine::FramebufferTextureFormat::RGBA8, SpoonEngine::FramebufferTextureFormat::Depth };
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        
        m_ViewportFramebuffer = SpoonEngine::Framebuffer::Create(fbSpec);
        m_ViewportSize = ImVec2(1280.0f, 720.0f);
        
        std::cout << "Created viewport framebuffer (1280x720)" << std::endl;
    }
    
    void UpdateMaterialsWithCameraPosition() {
        auto world = GetWorld();
        if (!m_CameraEntity.IsValid()) return;
        
        auto& cameraComp = world->GetComponent<SpoonEngine::CameraComponent>(m_CameraEntity);
        glm::vec3 cameraPos(
            cameraComp.Camera->GetPosition().x,
            cameraComp.Camera->GetPosition().y,
            cameraComp.Camera->GetPosition().z
        );
        
        for (auto entity : m_SceneEntities) {
            if (world->HasComponent<SpoonEngine::MeshRenderer>(entity)) {
                auto& meshRenderer = world->GetComponent<SpoonEngine::MeshRenderer>(entity);
                if (meshRenderer.Material) {
                    meshRenderer.Material->SetFloat3("u_CameraPosition", cameraPos);
                }
            }
        }
    }
    
    // Scene management methods
    void CreateNewScene() {
        auto world = GetWorld();
        if (!world) return;
        
        // Clear existing scene entities (keep camera)
        for (auto entity : m_SceneEntities) {
            world->DestroyEntity(entity);
        }
        m_SceneEntities.clear();
        
        m_CurrentScenePath = "";
        m_SelectedEntity = SpoonEngine::Entity();
        
        std::cout << "New scene created" << std::endl;
    }
    
    void SaveScene() {
        if (m_CurrentScenePath.empty()) {
            SaveSceneAs();
        } else {
            auto world = GetWorld();
            if (world) {
                std::cout << "Saving scene to existing path: " << m_CurrentScenePath << std::endl;
                if (SpoonEngine::SceneSerializer::SaveScene(world, m_SceneEntities, m_CurrentScenePath)) {
                    std::cout << "Scene saved successfully to: " << m_CurrentScenePath << std::endl;
                } else {
                    std::cerr << "Failed to save scene to: " << m_CurrentScenePath << std::endl;
                }
            }
        }
    }
    
    void SaveSceneAs() {
        // Just trigger the dialog - rendering happens in RenderEditorUI()
        m_ShowSaveAsDialog = true;
    }
    
    void LoadScene() {
        // Just trigger the dialog - rendering happens in RenderEditorUI()
        m_ShowOpenDialog = true;
    }
    
    void RefreshSceneEntities() {
        // Clear selection since entity IDs may have changed
        m_SelectedEntity = SpoonEngine::Entity();
        
        // The m_SceneEntities vector is now properly populated by the enhanced LoadScene method
        // So we just need to set up materials for the loaded entities
        SetupMaterialsForLoadedEntities();
        
        std::cout << "Scene entities refreshed - " << m_SceneEntities.size() << " entities loaded" << std::endl;
    }
    
    void SetupMaterialsForLoadedEntities() {
        auto world = GetWorld();
        if (!world) return;
        
        // Create shader for loaded entities (same as default scene)
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
        )";
        
        auto shader = SpoonEngine::Shader::Create("LoadedEntity", vertexShader, fragmentShader);
        
        // Default colors for loaded entities
        std::vector<SpoonEngine::Vector3> colors = {
            SpoonEngine::Vector3(0.8f, 0.3f, 0.3f),  // Red
            SpoonEngine::Vector3(0.3f, 0.8f, 0.3f),  // Green
            SpoonEngine::Vector3(0.3f, 0.3f, 0.8f),  // Blue
            SpoonEngine::Vector3(0.8f, 0.8f, 0.3f),  // Yellow
            SpoonEngine::Vector3(0.8f, 0.3f, 0.8f)   // Magenta
        };
        
        // Set up materials for each loaded entity
        for (size_t i = 0; i < m_SceneEntities.size(); i++) {
            auto entity = m_SceneEntities[i];
            if (!entity.IsValid() || !world->HasComponent<SpoonEngine::MeshRenderer>(entity)) continue;
            
            auto& meshRenderer = world->GetComponent<SpoonEngine::MeshRenderer>(entity);
            
            // Create material with appropriate color
            SpoonEngine::Vector3 color = colors[i % colors.size()];
            auto material = SpoonEngine::Material::Create(shader, "LoadedEntity" + std::to_string(i));
            material->SetFloat3("u_Color", glm::vec3(color.x, color.y, color.z));
            material->SetFloat3("u_LightPosition", glm::vec3(5.0f, 5.0f, 5.0f));
            material->SetFloat3("u_LightColor", glm::vec3(1.0f, 1.0f, 1.0f));
            
            meshRenderer.Material = material;
        }
        
        std::cout << "Materials set up for " << m_SceneEntities.size() << " loaded entities" << std::endl;
    }
    
    std::string GetScenesDirectory() {
        // Get current working directory
        std::filesystem::path cwd = std::filesystem::current_path();
        std::cout << "Current working directory: " << cwd << std::endl;
        
        // Method 1: Try to find project root by looking for CLAUDE.md or CMakeLists.txt
        std::filesystem::path projectRoot = cwd;
        for (int i = 0; i < 5; i++) {  // Look up to 5 levels up
            if (std::filesystem::exists(projectRoot / "CLAUDE.md") || 
                std::filesystem::exists(projectRoot / "CMakeLists.txt")) {
                std::cout << "Found project root: " << projectRoot << std::endl;
                break;
            }
            projectRoot = projectRoot.parent_path();
        }
        
        // Create scenes directory path
        std::filesystem::path scenesDir = projectRoot / "scenes";
        
        // Create directory if it doesn't exist
        try {
            std::filesystem::create_directories(scenesDir);
            std::cout << "Scenes directory: " << scenesDir << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Failed to create scenes directory: " << e.what() << std::endl;
            // Fallback to current directory
            scenesDir = cwd / "scenes";
            std::filesystem::create_directories(scenesDir);
            std::cout << "Using fallback scenes directory: " << scenesDir << std::endl;
        }
        
        return scenesDir.string();
    }
    
    void RenderSaveAsDialog() {
        if (m_ShowSaveAsDialog) {
            ImGui::OpenPopup("Save Scene As");
            m_ShowSaveAsDialog = false;  // Only open once
        }
        
        if (ImGui::BeginPopupModal("Save Scene As", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Enter filename:");
            ImGui::InputText("##filename", m_SaveFilename, sizeof(m_SaveFilename));
            
            ImGui::Separator();
            
            if (ImGui::Button("Save", ImVec2(120, 0))) {
                // Get proper scenes directory path
                std::string scenesDir = GetScenesDirectory();
                std::filesystem::path fullPath = std::filesystem::path(scenesDir) / m_SaveFilename;
                
                auto world = GetWorld();
                if (world) {
                    std::cout << "Attempting to save scene to: " << fullPath << std::endl;
                    
                    if (SpoonEngine::SceneSerializer::SaveScene(world, m_SceneEntities, fullPath.string())) {
                        m_CurrentScenePath = fullPath.string();
                        std::cout << "Scene saved successfully to: " << fullPath << std::endl;
                        std::cout << "File is located at: " << fullPath << std::endl;
                    } else {
                        std::cerr << "Failed to save scene to: " << fullPath << std::endl;
                    }
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndPopup();
        }
    }
    
    void RenderOpenDialog() {
        if (m_ShowOpenDialog) {
            ImGui::OpenPopup("Open Scene");
            m_ShowOpenDialog = false;  // Only open once
        }
        
        if (ImGui::BeginPopupModal("Open Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Enter filename to load:");
            ImGui::InputText("##loadfilename", m_OpenFilename, sizeof(m_OpenFilename));
            
            ImGui::Separator();
            
            if (ImGui::Button("Load", ImVec2(120, 0))) {
                // Get proper scenes directory path
                std::string scenesDir = GetScenesDirectory();
                std::filesystem::path fullPath = std::filesystem::path(scenesDir) / m_OpenFilename;
                
                auto world = GetWorld();
                if (world) {
                    std::cout << "Attempting to load scene from: " << fullPath << std::endl;
                    
                    if (SpoonEngine::SceneSerializer::LoadScene(world, m_SceneEntities, fullPath.string())) {
                        m_CurrentScenePath = fullPath.string();
                        RefreshSceneEntities();
                        std::cout << "Scene loaded successfully from: " << fullPath << std::endl;
                    } else {
                        std::cerr << "Failed to load scene from: " << fullPath << std::endl;
                    }
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndPopup();
        }
    }
    
private:
    std::shared_ptr<SpoonEngine::RenderSystem> m_RenderSystem;
    std::vector<SpoonEngine::Entity> m_SceneEntities;
    SpoonEngine::Entity m_CameraEntity;
    SpoonEngine::Entity m_SelectedEntity;
    
    // Viewport rendering
    std::shared_ptr<SpoonEngine::Framebuffer> m_ViewportFramebuffer;
    ImVec2 m_ViewportSize = ImVec2(0.0f, 0.0f);
    
    // Scene management
    std::string m_CurrentScenePath = "";
    
    // Dialog state
    bool m_ShowSaveAsDialog = false;
    bool m_ShowOpenDialog = false;
    char m_SaveFilename[256] = "scene.spoon";
    char m_OpenFilename[256] = "scene.spoon";
    
    // UI panel visibility
    bool m_ShowSceneHierarchy = true;
    bool m_ShowProperties = true;
    bool m_ShowViewport = true;
};

SPOON_MAIN(SpoonEditorApp)