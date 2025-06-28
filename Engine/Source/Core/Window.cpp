#include "SpoonEngine/Core/Window.h"

#include <GLFW/glfw3.h>
#include <iostream>

namespace SpoonEngine {
    
    static bool s_GLFWInitialized = false;
    
    static void GLFWErrorCallback(int error, const char* description) {
        std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
    }
    
    Window::Window(const WindowProps& props) {
        Init(props);
    }
    
    Window::~Window() {
        Shutdown();
    }
    
    void Window::Init(const WindowProps& props) {
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;
        m_Data.VSync = props.VSync;
        
        if (!s_GLFWInitialized) {
            int success = glfwInit();
            if (!success) {
                std::cerr << "Could not initialize GLFW!" << std::endl;
                return;
            }
            glfwSetErrorCallback(GLFWErrorCallback);
            s_GLFWInitialized = true;
        }
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        
        m_Window = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height), 
                                   m_Data.Title.c_str(), nullptr, nullptr);
        
        if (!m_Window) {
            std::cerr << "Failed to create GLFW window!" << std::endl;
            glfwTerminate();
            return;
        }
        
        glfwMakeContextCurrent(m_Window);
        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(props.VSync);
        
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;
        });
        
        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        });
    }
    
    void Window::Shutdown() {
        if (m_Window) {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }
    }
    
    void Window::OnUpdate() {
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }
    
    void Window::SetVSync(bool enabled) {
        if (enabled) {
            glfwSwapInterval(1);
        } else {
            glfwSwapInterval(0);
        }
        m_Data.VSync = enabled;
    }
    
    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(m_Window);
    }
    
}