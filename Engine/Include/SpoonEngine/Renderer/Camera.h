#pragma once

#include "SpoonEngine/Math/Vector3.h"
#include "SpoonEngine/Math/Matrix4.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SpoonEngine {
    
    enum class CameraType {
        Perspective,
        Orthographic
    };
    
    class Camera {
    public:
        Camera(CameraType type = CameraType::Perspective);
        virtual ~Camera() = default;
        
        void SetPosition(const Vector3& position);
        void SetRotation(const Vector3& rotation);
        void LookAt(const Vector3& target, const Vector3& up = Vector3::Up());
        
        void SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
        void SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
        
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
        
        const Vector3& GetPosition() const { return m_Position; }
        const Vector3& GetRotation() const { return m_Rotation; }
        virtual Vector3 GetForward() const;
        virtual Vector3 GetRight() const;
        virtual Vector3 GetUp() const;
        
        float GetFOV() const { return m_FOV; }
        float GetAspectRatio() const { return m_AspectRatio; }
        float GetNearPlane() const { return m_NearPlane; }
        float GetFarPlane() const { return m_FarPlane; }
        
        void Update();
        
    private:
        void RecalculateViewMatrix();
        void RecalculateProjectionMatrix();
        
        CameraType m_Type;
        Vector3 m_Position = Vector3::Zero();
        Vector3 m_Rotation = Vector3::Zero();
        
        // Perspective parameters
        float m_FOV = 45.0f;
        float m_AspectRatio = 16.0f / 9.0f;
        float m_NearPlane = 0.1f;
        float m_FarPlane = 1000.0f;
        
        // Orthographic parameters
        float m_Left = -10.0f;
        float m_Right = 10.0f;
        float m_Bottom = -10.0f;
        float m_Top = 10.0f;
        
        glm::mat4 m_ViewMatrix{1.0f};
        glm::mat4 m_ProjectionMatrix{1.0f};
        glm::mat4 m_ViewProjectionMatrix{1.0f};
    };
    
    class FlyCamera : public Camera {
    public:
        FlyCamera();
        
        void ProcessKeyboard(int key, float deltaTime);
        void ProcessMouseMovement(float xOffset, float yOffset);
        void ProcessMouseScroll(float yOffset);
        
        void SetMovementSpeed(float speed) { m_MovementSpeed = speed; }
        void SetMouseSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }
        
        float GetMovementSpeed() const { return m_MovementSpeed; }
        float GetMouseSensitivity() const { return m_MouseSensitivity; }
        
        // Override vector calculations to use FlyCamera's yaw/pitch
        Vector3 GetForward() const override;
        Vector3 GetRight() const override;
        Vector3 GetUp() const override;
        
    private:
        float m_MovementSpeed = 5.0f;
        float m_MouseSensitivity = 0.1f;
        float m_Yaw = -90.0f;
        float m_Pitch = 0.0f;
        
        void UpdateCameraVectors();
    };
    
}