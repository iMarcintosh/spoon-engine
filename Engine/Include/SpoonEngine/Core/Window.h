#pragma once

#include <string>
#include <functional>

struct GLFWwindow;

namespace SpoonEngine {
    
    struct WindowProps {
        std::string Title;
        uint32_t Width;
        uint32_t Height;
        bool VSync;
        
        WindowProps(const std::string& title = "SpoonEngine",
                   uint32_t width = 1280,
                   uint32_t height = 720,
                   bool vsync = true)
            : Title(title), Width(width), Height(height), VSync(vsync) {}
    };
    
    class Window {
    public:
        using EventCallbackFn = std::function<void(class Event&)>;
        
        Window(const WindowProps& props);
        virtual ~Window();
        
        void OnUpdate();
        
        uint32_t GetWidth() const { return m_Data.Width; }
        uint32_t GetHeight() const { return m_Data.Height; }
        
        void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
        void SetVSync(bool enabled);
        bool IsVSync() const { return m_Data.VSync; }
        
        GLFWwindow* GetNativeWindow() const { return m_Window; }
        
        bool ShouldClose() const;
        
    private:
        void Init(const WindowProps& props);
        void Shutdown();
        
        GLFWwindow* m_Window;
        
        struct WindowData {
            std::string Title;
            uint32_t Width, Height;
            bool VSync;
            EventCallbackFn EventCallback;
        };
        
        WindowData m_Data;
    };
    
}