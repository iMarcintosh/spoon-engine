#pragma once

#include "SpoonEngine/ECS/World.h"
#include "SpoonEngine/Components/Transform.h"
#include <memory>
#include <string>

struct GLFWwindow;

namespace SpoonEngine {
    
    class EditorLayer {
    public:
        EditorLayer();
        ~EditorLayer();
        
        void Initialize(GLFWwindow* window);
        void Shutdown();
        
        void OnUpdate(float deltaTime);
        void OnRender();
        
        void SetWorld(World* world) { m_World = world; }
        World* GetWorld() const { return m_World; }
        
        void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }
        Entity GetSelectedEntity() const { return m_SelectedEntity; }
        
        bool IsEditorEnabled() const { return m_EditorEnabled; }
        void SetEditorEnabled(bool enabled) { m_EditorEnabled = enabled; }
        
    private:
        void DrawMenuBar();
        void DrawSceneHierarchy();
        void DrawInspector();
        void DrawViewport();
        void DrawAssetBrowser();
        void DrawConsole();
        
        void DrawTransformComponent(Transform& transform);
        void DrawMeshRendererComponent(MeshRenderer& meshRenderer);
        
        void CreateEntity(const std::string& name);
        void DeleteEntity(Entity entity);
        void DuplicateEntity(Entity entity);
        
        World* m_World = nullptr;
        Entity m_SelectedEntity = {};
        bool m_EditorEnabled = true;
        
        // UI State
        bool m_ShowDemoWindow = false;
        bool m_ShowSceneHierarchy = true;
        bool m_ShowInspector = true;
        bool m_ShowViewport = true;
        bool m_ShowAssetBrowser = true;
        bool m_ShowConsole = true;
        
        // Viewport
        bool m_ViewportFocused = false;
        bool m_ViewportHovered = false;
        
        // Entity creation
        char m_EntityNameBuffer[256] = "New Entity";
    };
    
}