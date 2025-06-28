#pragma once

#include "Core/Application.h"
#include "Core/Window.h"
#include "Core/Input.h"

#include "ECS/Entity.h"
#include "ECS/Component.h"
#include "ECS/System.h"
#include "ECS/World.h"

#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"
#include "Renderer/Material.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4.h"

#include "Assets/AssetManager.h"
#include "Assets/Texture.h"
#include "Assets/Mesh.h"
#include "Assets/Model.h"
#include "Assets/SceneSerializer.h"

#include "Components/Transform.h"
#include "Systems/RenderSystem.h"

namespace SpoonEngine {
    extern Application* g_Application;
}

#define SPOON_MAIN(className) \
    SpoonEngine::Application* SpoonEngine::CreateApplication() { \
        return new className(); \
    } \
    SpoonEngine::Application* SpoonEngine::g_Application = nullptr; \
    int main(int argc, char** argv) { \
        SpoonEngine::g_Application = SpoonEngine::CreateApplication(); \
        SpoonEngine::g_Application->Run(); \
        delete SpoonEngine::g_Application; \
        return 0; \
    }