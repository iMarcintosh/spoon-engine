#include "SpoonEngine/Systems/RenderSystem.h"
#include "SpoonEngine/Components/Transform.h"
#include "SpoonEngine/Renderer/Camera.h"
#include "SpoonEngine/Renderer/Renderer.h"
#include "SpoonEngine/Renderer/Material.h"
#include "SpoonEngine/Assets/Mesh.h"
#include "SpoonEngine/Renderer/Shader.h"
#include "SpoonEngine/ECS/World.h"

namespace SpoonEngine {
    
    RenderSystem::RenderSystem() {
        m_Camera = std::make_shared<Camera>(CameraType::Perspective);
        m_Camera->SetPosition(Vector3(2, 2, 3));
        m_Camera->LookAt(Vector3::Zero());
    }
    
    void RenderSystem::Update(float deltaTime) {
        if (m_Camera) {
            m_Camera->Update();
        }
    }
    
    void RenderSystem::Render(Renderer* renderer) {
        if (!m_Camera || !m_World) return;
        
        for (auto entity : m_Entities) {
            if (m_World->HasComponent<Transform>(entity) && m_World->HasComponent<MeshRenderer>(entity)) {
                auto& transform = m_World->GetComponent<Transform>(entity);
                auto& meshRenderer = m_World->GetComponent<MeshRenderer>(entity);
                
                // Use material if available, otherwise fall back to legacy shader
                bool useMaterial = meshRenderer.Material && meshRenderer.Material->GetShader();
                bool useShader = meshRenderer.Shader && !useMaterial;
                
                if ((useMaterial || useShader) && meshRenderer.Mesh) {
                    // Set transformation matrices
                    glm::mat4 model = transform.GetTransformMatrix();
                    glm::mat4 view = m_Camera->GetViewMatrix();
                    glm::mat4 projection = m_Camera->GetProjectionMatrix();
                    
                    if (useMaterial) {
                        // Use material system
                        meshRenderer.Material->SetMat4("u_Model", model);
                        meshRenderer.Material->SetMat4("u_View", view);
                        meshRenderer.Material->SetMat4("u_Projection", projection);
                        meshRenderer.Material->SetFloat3("u_CameraPosition", 
                            glm::vec3(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z));
                        
                        meshRenderer.Material->Bind();
                    } else {
                        // Legacy shader system
                        meshRenderer.Shader->Bind();
                        meshRenderer.Shader->SetMat4("u_Model", model);
                        meshRenderer.Shader->SetMat4("u_View", view);
                        meshRenderer.Shader->SetMat4("u_Projection", projection);
                        meshRenderer.Shader->SetFloat3("u_CameraPosition", 
                            glm::vec3(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z));
                    }
                    
                    meshRenderer.Mesh->Bind();
                    renderer->DrawIndexed(meshRenderer.Mesh->GetIndexCount());
                    meshRenderer.Mesh->Unbind();
                    
                    if (useMaterial) {
                        meshRenderer.Material->Unbind();
                    } else {
                        meshRenderer.Shader->Unbind();
                    }
                }
            }
        }
    }
    
    CameraSystem::CameraSystem() {
        m_ActiveCamera = std::make_shared<FlyCamera>();
    }
    
    void CameraSystem::Update(float deltaTime) {
        UpdateActiveCamera();
        
        if (m_ActiveCamera) {
            m_ActiveCamera->Update();
        }
    }
    
    void CameraSystem::Render(Renderer* renderer) {
        // Camera system doesn't render, but updates camera state
    }
    
    void CameraSystem::UpdateActiveCamera() {
        if (!m_World) return;
        
        // Find the highest priority active camera
        std::shared_ptr<Camera> highestPriorityCamera = nullptr;
        int highestPriority = -1;
        
        for (auto entity : m_Entities) {
            if (m_World->HasComponent<CameraComponent>(entity)) {
                auto& cameraComp = m_World->GetComponent<CameraComponent>(entity);
                
                if (cameraComp.IsActive && cameraComp.Priority > highestPriority) {
                    highestPriority = cameraComp.Priority;
                    highestPriorityCamera = cameraComp.Camera;
                    
                    // Update camera position from transform if available
                    if (m_World->HasComponent<Transform>(entity)) {
                        auto& transform = m_World->GetComponent<Transform>(entity);
                        cameraComp.Camera->SetPosition(transform.Position);
                        cameraComp.Camera->SetRotation(transform.Rotation);
                    }
                }
            }
        }
        
        if (highestPriorityCamera) {
            m_ActiveCamera = highestPriorityCamera;
        }
    }
    
}