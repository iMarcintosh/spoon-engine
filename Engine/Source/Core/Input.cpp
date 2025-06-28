#include "SpoonEngine/Core/Input.h"
#include "SpoonEngine/Core/Application.h"
#include "SpoonEngine/Core/Window.h"

#include <GLFW/glfw3.h>

namespace SpoonEngine {
    
    extern Application* g_Application;
    
    bool Input::IsKeyPressed(KeyCode key) {
        auto window = g_Application->GetWindow()->GetNativeWindow();
        auto state = glfwGetKey(window, static_cast<int32_t>(key));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }
    
    bool Input::IsMouseButtonPressed(MouseButton button) {
        auto window = g_Application->GetWindow()->GetNativeWindow();
        auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
        return state == GLFW_PRESS;
    }
    
    glm::vec2 Input::GetMousePosition() {
        auto window = g_Application->GetWindow()->GetNativeWindow();
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return { static_cast<float>(xpos), static_cast<float>(ypos) };
    }
    
    float Input::GetMouseX() {
        return GetMousePosition().x;
    }
    
    float Input::GetMouseY() {
        return GetMousePosition().y;
    }
    
}