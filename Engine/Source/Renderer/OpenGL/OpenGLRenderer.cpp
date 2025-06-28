#include "SpoonEngine/Renderer/OpenGL/OpenGLRenderer.h"

#include <iostream>

#ifdef SPOON_PLATFORM_MACOS
    #define GL_SILENCE_DEPRECATION
    #include <OpenGL/gl3.h>
#else
    #include <GL/gl.h>
#endif

namespace SpoonEngine {
    
    void OpenGLRenderer::Initialize() {
        std::cout << "OpenGL Renderer initialized" << std::endl;
        glEnable(GL_DEPTH_TEST);
    }
    
    void OpenGLRenderer::Shutdown() {
        std::cout << "OpenGL Renderer shutdown" << std::endl;
    }
    
    void OpenGLRenderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
        glViewport(x, y, width, height);
    }
    
    void OpenGLRenderer::SetClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }
    
    void OpenGLRenderer::Clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    void OpenGLRenderer::DrawIndexed(uint32_t indexCount) {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    }
    
    void OpenGLRenderer::Present() {
    }
    
}