#include "SpoonEngine/Renderer/Camera.h"
#include "SpoonEngine/Core/Input.h"
#include <algorithm>

namespace SpoonEngine {
    
    Camera::Camera(CameraType type) 
        : m_Type(type) {
        RecalculateProjectionMatrix();
        RecalculateViewMatrix();
    }
    
    void Camera::SetPosition(const Vector3& position) {
        m_Position = position;
        RecalculateViewMatrix();
    }
    
    void Camera::SetRotation(const Vector3& rotation) {
        m_Rotation = rotation;
        RecalculateViewMatrix();
    }
    
    void Camera::LookAt(const Vector3& target, const Vector3& up) {
        glm::vec3 pos(m_Position.x, m_Position.y, m_Position.z);
        glm::vec3 tgt(target.x, target.y, target.z);
        glm::vec3 upVec(up.x, up.y, up.z);
        
        m_ViewMatrix = glm::lookAt(pos, tgt, upVec);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
    
    void Camera::SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane) {
        m_Type = CameraType::Perspective;
        m_FOV = fov;
        m_AspectRatio = aspectRatio;
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
        RecalculateProjectionMatrix();
    }
    
    void Camera::SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
        m_Type = CameraType::Orthographic;
        m_Left = left;
        m_Right = right;
        m_Bottom = bottom;
        m_Top = top;
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
        RecalculateProjectionMatrix();
    }
    
    Vector3 Camera::GetForward() const {
        glm::vec3 forward;
        forward.x = cos(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x));
        forward.y = sin(glm::radians(m_Rotation.x));
        forward.z = sin(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x));
        return Vector3(forward.x, forward.y, forward.z);
    }
    
    Vector3 Camera::GetRight() const {
        Vector3 forward = GetForward();
        Vector3 up = Vector3::Up();
        glm::vec3 f(forward.x, forward.y, forward.z);
        glm::vec3 u(up.x, up.y, up.z);
        glm::vec3 right = glm::normalize(glm::cross(f, u));
        return Vector3(right.x, right.y, right.z);
    }
    
    Vector3 Camera::GetUp() const {
        Vector3 forward = GetForward();
        Vector3 right = GetRight();
        glm::vec3 f(forward.x, forward.y, forward.z);
        glm::vec3 r(right.x, right.y, right.z);
        glm::vec3 up = glm::cross(r, f);
        return Vector3(up.x, up.y, up.z);
    }
    
    void Camera::Update() {
        RecalculateViewMatrix();
    }
    
    void Camera::RecalculateViewMatrix() {
        glm::mat4 rotation = glm::mat4(1.0f);
        rotation = glm::rotate(rotation, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
        rotation = glm::rotate(rotation, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
        rotation = glm::rotate(rotation, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
        
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), 
            glm::vec3(-m_Position.x, -m_Position.y, -m_Position.z));
        
        m_ViewMatrix = rotation * translation;
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
    
    void Camera::RecalculateProjectionMatrix() {
        if (m_Type == CameraType::Perspective) {
            m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
        } else {
            m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_NearPlane, m_FarPlane);
        }
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
    
    // FlyCamera implementation
    FlyCamera::FlyCamera() : Camera(CameraType::Perspective) {
        SetPosition(Vector3(0, 0, 3));
        // Set initial orientation to look forward (negative Z direction)
        m_Yaw = -90.0f;  // Look down negative Z axis
        m_Pitch = 0.0f;  // Level horizon
        UpdateCameraVectors();
    }
    
    void FlyCamera::ProcessKeyboard(int key, float deltaTime) {
        float velocity = m_MovementSpeed * deltaTime;
        Vector3 forward = GetForward();
        Vector3 right = GetRight();
        Vector3 up = Vector3::Up();
        
        Vector3 position = GetPosition();
        
        if (key == static_cast<int>(KeyCode::W)) {
            position += forward * velocity;
        }
        if (key == static_cast<int>(KeyCode::S)) {
            position -= forward * velocity;
        }
        if (key == static_cast<int>(KeyCode::A)) {
            position -= right * velocity;
        }
        if (key == static_cast<int>(KeyCode::D)) {
            position += right * velocity;
        }
        if (key == static_cast<int>(KeyCode::Q)) {
            position -= up * velocity;
        }
        if (key == static_cast<int>(KeyCode::E)) {
            position += up * velocity;
        }
        
        SetPosition(position);
    }
    
    void FlyCamera::ProcessMouseMovement(float xOffset, float yOffset) {
        xOffset *= m_MouseSensitivity;
        yOffset *= m_MouseSensitivity;
        
        m_Yaw += xOffset;
        m_Pitch += yOffset;
        
        // Constrain pitch
        m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);
        
        SetRotation(Vector3(m_Pitch, m_Yaw, 0.0f));
        UpdateCameraVectors();
    }
    
    void FlyCamera::ProcessMouseScroll(float yOffset) {
        float fov = GetFOV() - yOffset;
        fov = std::clamp(fov, 1.0f, 120.0f);
        SetPerspective(fov, GetAspectRatio(), GetNearPlane(), GetFarPlane());
    }
    
    Vector3 FlyCamera::GetForward() const {
        // Calculate forward vector using yaw and pitch (corrected coordinate system)
        glm::vec3 forward;
        forward.x = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        forward.y = sin(glm::radians(m_Pitch));
        forward.z = -cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        forward = glm::normalize(forward);
        return Vector3(forward.x, forward.y, forward.z);
    }
    
    Vector3 FlyCamera::GetRight() const {
        // Calculate right vector from forward and world up
        Vector3 forward = GetForward();
        Vector3 worldUp = Vector3::Up();
        glm::vec3 f(forward.x, forward.y, forward.z);
        glm::vec3 up(worldUp.x, worldUp.y, worldUp.z);
        glm::vec3 right = glm::normalize(glm::cross(f, up));
        return Vector3(right.x, right.y, right.z);
    }
    
    Vector3 FlyCamera::GetUp() const {
        // Calculate up vector from right and forward
        Vector3 forward = GetForward();
        Vector3 right = GetRight();
        glm::vec3 f(forward.x, forward.y, forward.z);
        glm::vec3 r(right.x, right.y, right.z);
        glm::vec3 up = glm::normalize(glm::cross(r, f));
        return Vector3(up.x, up.y, up.z);
    }
    
    void FlyCamera::UpdateCameraVectors() {
        // Update the base camera's rotation to match yaw/pitch
        SetRotation(Vector3(m_Pitch, m_Yaw, 0.0f));
        Update();
    }
    
}