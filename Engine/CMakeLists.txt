set(ENGINE_SOURCES
    Source/Core/Application.cpp
    Source/Core/Window.cpp
    Source/Core/Input.cpp
    Source/ECS/Entity.cpp
    Source/ECS/Component.cpp
    Source/ECS/System.cpp
    Source/ECS/World.cpp
    Source/Renderer/Renderer.cpp
    Source/Renderer/Camera.cpp
    Source/Renderer/Material.cpp
    Source/Renderer/OpenGL/OpenGLRenderer.cpp
    Source/Renderer/OpenGL/OpenGLShader.cpp
    Source/Renderer/OpenGL/OpenGLBuffer.cpp
    Source/Renderer/OpenGL/OpenGLTexture.cpp
    Source/Renderer/OpenGL/OpenGLFramebuffer.cpp
    Source/Renderer/Framebuffer.cpp
    Source/Renderer/Shader.cpp
    Source/Renderer/Buffer.cpp
    Source/Math/Vector2.cpp
    Source/Math/Vector3.cpp
    Source/Math/Vector4.cpp
    Source/Math/Matrix4.cpp
    Source/Assets/AssetManager.cpp
    Source/Assets/Texture.cpp
    Source/Assets/Mesh.cpp
    Source/Assets/Model.cpp
    Source/Assets/ModelLoader.cpp
    Source/Assets/SceneSerializer.cpp
    Source/Systems/RenderSystem.cpp
)

set(ENGINE_HEADERS
    Include/SpoonEngine/Core/Application.h
    Include/SpoonEngine/Core/Window.h
    Include/SpoonEngine/Core/Input.h
    Include/SpoonEngine/ECS/Entity.h
    Include/SpoonEngine/ECS/Component.h
    Include/SpoonEngine/ECS/System.h
    Include/SpoonEngine/ECS/World.h
    Include/SpoonEngine/Renderer/Renderer.h
    Include/SpoonEngine/Renderer/Camera.h
    Include/SpoonEngine/Renderer/Material.h
    Include/SpoonEngine/Renderer/OpenGL/OpenGLRenderer.h
    Include/SpoonEngine/Renderer/OpenGL/OpenGLShader.h
    Include/SpoonEngine/Renderer/OpenGL/OpenGLBuffer.h
    Include/SpoonEngine/Renderer/OpenGL/OpenGLTexture.h
    Include/SpoonEngine/Renderer/OpenGL/OpenGLFramebuffer.h
    Include/SpoonEngine/Renderer/Framebuffer.h
    Include/SpoonEngine/Renderer/Shader.h
    Include/SpoonEngine/Renderer/Buffer.h
    Include/SpoonEngine/Math/Vector2.h
    Include/SpoonEngine/Math/Vector3.h
    Include/SpoonEngine/Math/Vector4.h
    Include/SpoonEngine/Math/Matrix4.h
    Include/SpoonEngine/Assets/AssetManager.h
    Include/SpoonEngine/Assets/Texture.h
    Include/SpoonEngine/Assets/Mesh.h
    Include/SpoonEngine/Assets/Model.h
    Include/SpoonEngine/Assets/ModelLoader.h
    Include/SpoonEngine/Assets/SceneSerializer.h
    Include/SpoonEngine/Components/Transform.h
    Include/SpoonEngine/Systems/RenderSystem.h
    Include/SpoonEngine/SpoonEngine.h
)

find_package(glm QUIET)
if(NOT glm_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8
    )
    FetchContent_MakeAvailable(glm)
endif()

# ImGui
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.1
)
FetchContent_MakeAvailable(imgui)

set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(SpoonEngine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS} ${IMGUI_SOURCES})

target_include_directories(SpoonEngine 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(SpoonEngine 
    PUBLIC 
        OpenGL::GL
        glfw
        glm::glm
)

if(WIN32)
    target_compile_definitions(SpoonEngine PRIVATE SPOON_PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(SpoonEngine PRIVATE SPOON_PLATFORM_MACOS)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(SpoonEngine PUBLIC ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
elseif(UNIX)
    target_compile_definitions(SpoonEngine PRIVATE SPOON_PLATFORM_LINUX)
endif()