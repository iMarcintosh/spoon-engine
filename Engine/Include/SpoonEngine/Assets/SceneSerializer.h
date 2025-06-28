#pragma once

#include "SpoonEngine/ECS/World.h"
#include "SpoonEngine/ECS/Entity.h"
#include "SpoonEngine/Math/Vector3.h"
#include <string>
#include <memory>
#include <vector>

// Forward declarations
namespace SpoonEngine {
    struct Transform;
    struct MeshRenderer;
    struct CameraComponent;
}

namespace SpoonEngine {
    
    class SceneSerializer {
    public:
        SceneSerializer() = default;
        ~SceneSerializer() = default;
        
        // Serialize entire scene to JSON file
        static bool SaveScene(World* world, const std::string& filepath);
        static bool SaveScene(World* world, const std::vector<Entity>& entities, const std::string& filepath);
        
        // Deserialize scene from JSON file
        static bool LoadScene(World* world, const std::string& filepath);
        static bool LoadScene(World* world, std::vector<Entity>& entities, const std::string& filepath);
        
        // Serialize scene to JSON string
        static std::string SerializeScene(World* world);
        static std::string SerializeScene(World* world, const std::vector<Entity>& entities);
        
        // Deserialize scene from JSON string
        static bool DeserializeScene(World* world, const std::string& jsonData);
        static bool DeserializeScene(World* world, std::vector<Entity>& entities, const std::string& jsonData);
        
    private:
        // Helper methods for component serialization
        static std::string SerializeTransform(const Transform& transform);
        static std::string SerializeMeshRenderer(const MeshRenderer& meshRenderer);
        static std::string SerializeCameraComponent(const CameraComponent& camera);
        
        // Helper methods for component deserialization
        static Transform DeserializeTransform(const std::string& jsonData);
        static MeshRenderer DeserializeMeshRenderer(const std::string& jsonData);
        static CameraComponent DeserializeCameraComponent(const std::string& jsonData);
        
        // Utility methods
        static std::string Vector3ToString(const Vector3& vec);
        static Vector3 StringToVector3(const std::string& str);
        static void CreateTestEntity(World* world, const Vector3& position, const Vector3& color);
        static Entity CreateEntityFromData(World* world, const Vector3& position, const Vector3& color);
    };
    
}