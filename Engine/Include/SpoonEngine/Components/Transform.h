#pragma once

#include "SpoonEngine/Math/Vector3.h"
#include "SpoonEngine/Math/Matrix4.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SpoonEngine {
    
    struct Transform {
        Vector3 Position = Vector3::Zero();
        Vector3 Rotation = Vector3::Zero(); // Euler angles in degrees
        Vector3 Scale = Vector3::One();
        
        Transform() = default;
        Transform(const Vector3& position) : Position(position) {}
        Transform(const Vector3& position, const Vector3& rotation) 
            : Position(position), Rotation(rotation) {}
        Transform(const Vector3& position, const Vector3& rotation, const Vector3& scale)
            : Position(position), Rotation(rotation), Scale(scale) {}
        
        glm::mat4 GetTransformMatrix() const {
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), 
                glm::vec3(Position.x, Position.y, Position.z));
            
            glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), 
                glm::radians(Rotation.x), glm::vec3(1, 0, 0));
            glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), 
                glm::radians(Rotation.y), glm::vec3(0, 1, 0));
            glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), 
                glm::radians(Rotation.z), glm::vec3(0, 0, 1));
            glm::mat4 rotation = rotationZ * rotationY * rotationX;
            
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), 
                glm::vec3(Scale.x, Scale.y, Scale.z));
            
            return translation * rotation * scale;
        }
        
        Vector3 GetForward() const {
            glm::vec3 forward;
            forward.x = cos(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.x));
            forward.y = sin(glm::radians(Rotation.x));
            forward.z = sin(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.x));
            return Vector3(forward.x, forward.y, forward.z);
        }
        
        Vector3 GetRight() const {
            Vector3 forward = GetForward();
            Vector3 up = Vector3::Up();
            glm::vec3 f(forward.x, forward.y, forward.z);
            glm::vec3 u(up.x, up.y, up.z);
            glm::vec3 right = glm::normalize(glm::cross(f, u));
            return Vector3(right.x, right.y, right.z);
        }
        
        Vector3 GetUp() const {
            Vector3 forward = GetForward();
            Vector3 right = GetRight();
            glm::vec3 f(forward.x, forward.y, forward.z);
            glm::vec3 r(right.x, right.y, right.z);
            glm::vec3 up = glm::cross(r, f);
            return Vector3(up.x, up.y, up.z);
        }
        
        void Translate(const Vector3& translation) {
            Position += translation;
        }
        
        void Rotate(const Vector3& eulerAngles) {
            Rotation += eulerAngles;
        }
        
        void LookAt(const Vector3& target, const Vector3& up = Vector3::Up()) {
            glm::vec3 pos(Position.x, Position.y, Position.z);
            glm::vec3 tgt(target.x, target.y, target.z);
            glm::vec3 upVec(up.x, up.y, up.z);
            
            glm::vec3 direction = glm::normalize(tgt - pos);
            float pitch = glm::degrees(asin(-direction.y));
            float yaw = glm::degrees(atan2(direction.x, direction.z));
            
            Rotation = Vector3(pitch, yaw, 0.0f);
        }
    };
    
    struct MeshRenderer {
        std::shared_ptr<class Mesh> Mesh;
        std::shared_ptr<class Material> Material;
        bool CastShadows = true;
        bool ReceiveShadows = true;
        
        // Legacy shader support for backward compatibility
        std::shared_ptr<class Shader> Shader;
    };
    
    struct CameraComponent {
        std::shared_ptr<class Camera> Camera;
        bool IsActive = true;
        int Priority = 0; // Higher priority cameras render last
    };
    
}