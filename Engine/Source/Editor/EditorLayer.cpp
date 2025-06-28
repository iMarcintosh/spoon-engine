#include "SpoonEngine/Editor/EditorLayer.h"
#include "SpoonEngine/Assets/Mesh.h"
#include "SpoonEngine/Renderer/Shader.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace SpoonEngine {
    
    EditorLayer::EditorLayer() {
    }
    
    EditorLayer::~EditorLayer() {
    }
    
    void EditorLayer::Initialize(GLFWwindow* window) {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        
        // When viewports are enabled we tweak WindowRounding/WindowBg
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
        
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        
        std::cout << "Editor Layer initialized" << std::endl;
    }
    
    void EditorLayer::Shutdown() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    
    void EditorLayer::OnUpdate(float deltaTime) {
        if (!m_EditorEnabled) return;
        
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    
    void EditorLayer::OnRender() {
        if (!m_EditorEnabled) return;
        
        static bool dockspaceOpen = true;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        
        // DockSpace
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);
        
        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        
        DrawMenuBar();
        
        if (m_ShowSceneHierarchy) DrawSceneHierarchy();
        if (m_ShowInspector) DrawInspector();
        if (m_ShowViewport) DrawViewport();
        if (m_ShowAssetBrowser) DrawAssetBrowser();
        if (m_ShowConsole) DrawConsole();
        
        if (m_ShowDemoWindow) ImGui::ShowDemoWindow(&m_ShowDemoWindow);
        
        ImGui::End();
        
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
    
    void EditorLayer::DrawMenuBar() {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
                    // TODO: Implement new scene
                }
                if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {
                    // TODO: Implement open scene
                }
                if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
                    // TODO: Implement save scene
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) {
                    // TODO: Implement exit
                }
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                    // TODO: Implement undo
                }
                if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
                    // TODO: Implement redo
                }
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("GameObject")) {
                if (ImGui::MenuItem("Create Empty")) {
                    CreateEntity("Empty GameObject");
                }
                if (ImGui::BeginMenu("3D Object")) {
                    if (ImGui::MenuItem("Cube")) {
                        CreateEntity("Cube");
                        // TODO: Add cube mesh
                    }
                    if (ImGui::MenuItem("Sphere")) {
                        CreateEntity("Sphere");
                        // TODO: Add sphere mesh
                    }
                    if (ImGui::MenuItem("Plane")) {
                        CreateEntity("Plane");
                        // TODO: Add plane mesh
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("Window")) {
                ImGui::MenuItem("Scene Hierarchy", nullptr, &m_ShowSceneHierarchy);
                ImGui::MenuItem("Inspector", nullptr, &m_ShowInspector);
                ImGui::MenuItem("Viewport", nullptr, &m_ShowViewport);
                ImGui::MenuItem("Asset Browser", nullptr, &m_ShowAssetBrowser);
                ImGui::MenuItem("Console", nullptr, &m_ShowConsole);
                ImGui::Separator();
                ImGui::MenuItem("ImGui Demo", nullptr, &m_ShowDemoWindow);
                ImGui::EndMenu();
            }
            
            ImGui::EndMenuBar();
        }
    }
    
    void EditorLayer::DrawSceneHierarchy() {
        ImGui::Begin("Scene Hierarchy", &m_ShowSceneHierarchy);
        
        if (ImGui::Button("Create Entity")) {
            CreateEntity(m_EntityNameBuffer);
        }
        ImGui::SameLine();
        ImGui::InputText("##EntityName", m_EntityNameBuffer, sizeof(m_EntityNameBuffer));
        
        ImGui::Separator();
        
        if (m_World) {
            // TODO: Iterate through entities and display hierarchy
            ImGui::Text("Entities: (System needs entity names)");
            if (ImGui::Selectable("Entity 1", m_SelectedEntity.GetID() == 1)) {
                m_SelectedEntity = Entity(1);
            }
            if (ImGui::Selectable("Entity 2", m_SelectedEntity.GetID() == 2)) {
                m_SelectedEntity = Entity(2);
            }
        }
        
        ImGui::End();
    }
    
    void EditorLayer::DrawInspector() {
        ImGui::Begin("Inspector", &m_ShowInspector);
        
        if (m_SelectedEntity.IsValid() && m_World) {
            ImGui::Text("Entity ID: %u", m_SelectedEntity.GetID());
            ImGui::Separator();
            
            // Transform Component
            if (m_World->HasComponent<Transform>(m_SelectedEntity)) {
                if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                    auto& transform = m_World->GetComponent<Transform>(m_SelectedEntity);
                    DrawTransformComponent(transform);
                }
            }
            
            // MeshRenderer Component
            if (m_World->HasComponent<MeshRenderer>(m_SelectedEntity)) {
                if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
                    auto& meshRenderer = m_World->GetComponent<MeshRenderer>(m_SelectedEntity);
                    DrawMeshRendererComponent(meshRenderer);
                }
            }
            
            ImGui::Separator();
            if (ImGui::Button("Add Component")) {
                ImGui::OpenPopup("AddComponent");
            }
            
            if (ImGui::BeginPopup("AddComponent")) {
                if (ImGui::MenuItem("Transform")) {
                    if (!m_World->HasComponent<Transform>(m_SelectedEntity)) {
                        m_World->AddComponent<Transform>(m_SelectedEntity, Transform());
                    }
                }
                if (ImGui::MenuItem("Mesh Renderer")) {
                    if (!m_World->HasComponent<MeshRenderer>(m_SelectedEntity)) {
                        m_World->AddComponent<MeshRenderer>(m_SelectedEntity, MeshRenderer());
                    }
                }
                ImGui::EndPopup();
            }
        } else {
            ImGui::Text("No entity selected");
        }
        
        ImGui::End();
    }
    
    void EditorLayer::DrawViewport() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport", &m_ShowViewport);
        
        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        ImGui::Text("Viewport Size: %.0f x %.0f", viewportPanelSize.x, viewportPanelSize.y);
        
        // TODO: Render scene to framebuffer and display here
        ImGui::Text("Scene rendering goes here");
        
        ImGui::End();
        ImGui::PopStyleVar();
    }
    
    void EditorLayer::DrawAssetBrowser() {
        ImGui::Begin("Asset Browser", &m_ShowAssetBrowser);
        
        ImGui::Text("Assets:");
        ImGui::Separator();
        
        // TODO: Display project assets
        ImGui::Text("Project assets will be displayed here");
        
        ImGui::End();
    }
    
    void EditorLayer::DrawConsole() {
        ImGui::Begin("Console", &m_ShowConsole);
        
        ImGui::Text("Console Output:");
        ImGui::Separator();
        
        // TODO: Implement console logging
        ImGui::TextWrapped("Console messages will appear here");
        
        ImGui::End();
    }
    
    void EditorLayer::DrawTransformComponent(Transform& transform) {
        ImGui::DragFloat3("Position", &transform.Position.x, 0.1f);
        ImGui::DragFloat3("Rotation", &transform.Rotation.x, 1.0f);
        ImGui::DragFloat3("Scale", &transform.Scale.x, 0.1f);
    }
    
    void EditorLayer::DrawMeshRendererComponent(MeshRenderer& meshRenderer) {
        ImGui::Text("Mesh: %s", meshRenderer.Mesh ? "Loaded" : "None");
        ImGui::Text("Shader: %s", meshRenderer.Shader ? "Loaded" : "None");
        ImGui::Checkbox("Cast Shadows", &meshRenderer.CastShadows);
        ImGui::Checkbox("Receive Shadows", &meshRenderer.ReceiveShadows);
    }
    
    void EditorLayer::CreateEntity(const std::string& name) {
        if (m_World) {
            Entity entity = m_World->CreateEntity();
            m_World->AddComponent<Transform>(entity, Transform());
            m_SelectedEntity = entity;
            std::cout << "Created entity: " << name << " (ID: " << entity.GetID() << ")" << std::endl;
        }
    }
    
    void EditorLayer::DeleteEntity(Entity entity) {
        if (m_World && entity.IsValid()) {
            m_World->DestroyEntity(entity);
            if (m_SelectedEntity == entity) {
                m_SelectedEntity = Entity();
            }
        }
    }
    
    void EditorLayer::DuplicateEntity(Entity entity) {
        // TODO: Implement entity duplication
    }
    
}