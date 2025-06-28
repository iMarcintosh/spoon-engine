# SpoonEngine

A modern, professional-grade multiplatform 2D/3D game engine written in C++ with Entity Component System (ECS) architecture. Built with production-quality rendering pipeline including advanced features like shadow mapping, material systems, and framebuffer-based render targets.

## 🎯 Key Features

### **Core Engine Architecture**
- **Multiplatform Support**: Windows, macOS, and Linux with native build systems
- **Entity Component System**: Unity-like ECS architecture for flexible, data-oriented game object management
- **Modern C++17**: Clean, professional codebase following industry best practices
- **Modular Design**: Plugin-ready architecture with clear separation of concerns

### **Advanced Rendering Pipeline**
- **Multiple Rendering APIs**: OpenGL 3.3+ (implemented), with planned Vulkan, DirectX, and Metal support
- **Professional Material System**: PBR-like materials with texture mapping and shader abstraction
- **Shadow Mapping**: Real-time shadow casting with PCF soft shadows and 2048x2048 shadow maps
- **Framebuffer System**: Render-to-texture capabilities for post-processing and advanced effects
- **Multi-Pass Rendering**: Depth-only shadow pass + main lighting pass architecture

### **Asset Pipeline & Management**
- **Advanced Texture Loading**: stb_image integration with multiple format support (RGB8, RGBA8, R8)
- **Mesh Management**: Procedural geometry generation and efficient GPU buffer management
- **Shader System**: Dynamic shader compilation with uniform management
- **Asset Caching**: Intelligent loading and memory management for optimal performance

### **Camera & Input Systems**
- **Professional Camera System**: FlyCamera implementation with WASD movement and mouse look
- **Input Abstraction**: Cross-platform keyboard and mouse input handling
- **Perspective & Orthographic**: Full camera projection support for 2D and 3D rendering

### **Editor & Development Tools**
- **Professional Scene Editor**: Complete ImGui-based editor with real-time viewport rendering
- **Scene Management**: Hierarchy panel, properties editor, and entity selection
- **Embedded 3D Viewport**: Real-time scene rendering with interactive camera controls
- **Asset Integration**: Full integration with engine's material and model loading systems

## 📊 Current Status

### ✅ **Production-Ready Features** (24 Major Components)

#### **Core Engine Foundation**
- [x] **Project Structure & Build System** - Professional CMake configuration with cross-platform support
- [x] **Application Framework** - Game loop, window management, event handling
- [x] **Entity Component System** - Complete ECS with component registration, system management
- [x] **Memory Management** - Efficient component storage and entity lifecycle management

#### **Advanced Rendering Pipeline**
- [x] **OpenGL Renderer Backend** - Modern OpenGL 3.3+ with VAO/VBO management
- [x] **Shader System** - Dynamic compilation, uniform management, hot-reloading capability
- [x] **Material System** - PBR-like materials with texture binding and parameter management
- [x] **Framebuffer System** - Render-to-texture with multiple attachment support
- [x] **Shadow Mapping** - Real-time directional shadows with PCF filtering
- [x] **Texture Loading** - stb_image integration with automatic format detection
- [x] **Camera System** - FlyCamera with perspective/orthographic projection
- [x] **Lighting System** - Phong/Blinn-Phong with ambient, diffuse, specular components

#### **Asset & Scene Management**
- [x] **Asset Loading Pipeline** - Centralized asset management with caching
- [x] **Mesh Generation** - Procedural geometry (cubes, planes) with UV mapping
- [x] **Transform System** - 3D transformations with matrix calculations
- [x] **Render System** - Component-based rendering with material support
- [x] **Scene Management** - World-based entity organization and system coordination

#### **Platform Integration**
- [x] **Input System** - Cross-platform keyboard/mouse input abstraction
- [x] **Window Management** - GLFW integration with context creation
- [x] **Math Library** - Vector2/3/4, Matrix4 with GLM integration
- [x] **Cross-Platform Build** - CMake with automatic dependency fetching
- [x] **Model Loading System** - Complete 3D asset import with OBJ format support
- [x] **Editor Application** - Professional ImGui-based scene editor with embedded viewport
- [x] **Scene Serialization System** - Complete JSON-based save/load with full entity hierarchy preservation

### 🚧 **In Progress**
- [ ] **Asset Browser Implementation** - File system integration for editor with drag-drop support

### 📋 **Planned Advanced Features**

#### **High Priority (Core Editor Enhancement)**
- [ ] **Asset Browser** - File system integration for editor with drag-drop support and texture preview
- [ ] **Entity Management Tools** - Runtime creation/deletion of entities with component templates
- [ ] **Enhanced Material Serialization** - Preserve actual material colors and properties in scene files

#### **Medium Priority (Editor Polish)**
- [ ] **Gizmos & Transform Handles** - Visual 3D manipulation tools for position/rotation/scale
- [ ] **Grid Rendering System** - Visual reference grid for 3D viewport with snap-to-grid
- [ ] **JSON Library Integration** - Replace basic string parsing with proper JSON library

#### **Future Advanced Features**
- [ ] **Advanced Model Formats** - GLTF, FBX support for model loading pipeline
- [ ] **C# Scripting Integration** - Mono/.NET runtime for game logic
- [ ] **Additional Renderer Backends** - Vulkan, DirectX 11/12, Metal implementations
- [ ] **Audio System** - 3D spatial audio with multiple format support
- [ ] **Physics Integration** - Bullet Physics or custom collision detection
- [ ] **Post-Processing Pipeline** - Bloom, SSAO, tone mapping, anti-aliasing
- [ ] **PBR Material System** - Physically Based Rendering with IBL

## 🛠️ Building

### Prerequisites
- **CMake 3.16+** - Cross-platform build system
- **C++17 Compatible Compiler** - GCC 8+, Clang 10+, MSVC 2019+
- **OpenGL 3.3+** - Modern graphics driver support
- **Git** - For dependency fetching

### Dependencies (Automatically Fetched)
- **GLFW 3.3+** - Window and context management
- **GLM 0.9.9+** - Mathematics library for graphics
- **stb_image** - Image loading library
- **ImGui 1.90+** - Immediate mode GUI (for editor)

### Build Instructions

```bash
# Clone the repository
git clone <repository-url>
cd spoon-engine

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the engine and examples
make -j$(nproc)  # Linux/macOS
# OR
cmake --build . --config Release  # Cross-platform
```

### Platform-Specific Notes

#### **macOS**
```bash
# Ensure Xcode command line tools are installed
xcode-select --install

# Build with OpenGL deprecation warnings silenced
make -j$(sysctl -n hw.ncpu)
```

#### **Windows**
```bash
# Use Visual Studio 2019+ or MinGW-w64
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

#### **Linux**
```bash
# Install development packages
sudo apt update
sudo apt install build-essential cmake libgl1-mesa-dev libglu1-mesa-dev

# Build
make -j$(nproc)
```

## 🎮 Running Examples

After building, you can run any of the professional demo applications:

### **BasicExample** - Engine Foundation Demo
```bash
./bin/BasicExample
```
- **Features**: Basic window, clear color, input handling
- **Purpose**: Demonstrates core engine initialization and game loop

### **CubeExample** - 3D Rendering Demo
```bash
./bin/CubeExample
```
- **Features**: Multiple rotating 3D cubes, camera movement, lighting
- **Purpose**: Shows 3D rendering pipeline, ECS in action, and camera system
- **Controls**: WASD for movement, QE for up/down, ESC to exit

### **TexturedCubeExample** - Material System Demo
```bash
./bin/TexturedCubeExample
```
- **Features**: Textured cubes with procedural checkerboard, material variations
- **Purpose**: Demonstrates advanced material system and texture loading
- **Technical**: Different metallic/roughness values per cube

### **ShadowMappingExample** - Advanced Rendering Demo
```bash
./bin/ShadowMappingExample
```
- **Features**: Real-time shadow mapping, animated lighting, multiple objects
- **Purpose**: Shows framebuffer system, shadow mapping, and multi-pass rendering
- **Technical**: 2048x2048 shadow maps, PCF filtering, dynamic light movement

### **ModelLoadingExample** - 3D Asset Pipeline Demo
```bash
./bin/ModelLoadingExample
```
- **Features**: OBJ model loading, procedural model generation, multiple instances
- **Purpose**: Demonstrates complete 3D asset pipeline with model parsing
- **Technical**: OBJ/MTL parsing, vertex processing, bounding box calculation, material integration

### **SpoonEditor** - Professional Scene Editor
```bash
./bin/SpoonEditor
```
- **Features**: Real-time 3D viewport, scene hierarchy, properties panel, WASD camera controls, complete scene save/load system
- **Purpose**: Complete scene editing environment with embedded rendering and project management
- **Technical**: ImGui integration, framebuffer viewport, entity selection, property editing, JSON-based scene serialization, cross-platform file handling

## 🏗️ Architecture

### Entity Component System (ECS)

SpoonEngine implements a high-performance, data-oriented ECS architecture:

```cpp
// Register components
auto world = GetWorld();
world->RegisterComponent<Transform>();
world->RegisterComponent<MeshRenderer>();

// Create entity
auto entity = world->CreateEntity();

// Add components
Transform transform;
transform.Position = Vector3(0, 1, 0);
world->AddComponent(entity, transform);

// Register and configure systems
auto renderSystem = world->RegisterSystem<RenderSystem>();
Signature signature;
signature.set(world->GetComponentManager()->GetComponentType<Transform>());
signature.set(world->GetComponentManager()->GetComponentType<MeshRenderer>());
world->SetSystemSignature<RenderSystem>(signature);
```

### Renderer Architecture

The rendering system uses a backend abstraction for multi-API support:

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Application   │───→│   RenderSystem   │───→│    Renderer     │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                                                         │
                       ┌─────────────────────────────────┼─────────────────┐
                       ▼                                 ▼                 ▼
               ┌──────────────┐                 ┌──────────────┐   ┌──────────────┐
               │ OpenGLRenderer│                 │VulkanRenderer│   │DirectXRenderer│
               └──────────────┘                 └──────────────┘   └──────────────┘
```

### Material System

Professional material workflow with shader abstraction:

```cpp
// Create advanced material
auto material = Material::Create(shader, "MaterialName");
material->SetTexture("u_MainTexture", diffuseTexture);
material->SetFloat3("u_Color", glm::vec3(1.0f, 0.5f, 0.2f));
material->SetFloat("u_Metallic", 0.8f);
material->SetFloat("u_Roughness", 0.2f);

// Bind and render
material->Bind();
// Rendering happens automatically through RenderSystem
material->Unbind();
```

### Framebuffer System

Advanced render-to-texture capabilities:

```cpp
// Create shadow map framebuffer
FramebufferSpecification spec;
spec.Width = 2048;
spec.Height = 2048;
spec.Attachments = { FramebufferTextureFormat::DEPTH24STENCIL8 };

auto shadowMapFB = Framebuffer::Create(spec);

// Render shadow pass
shadowMapFB->Bind();
// ... render scene from light perspective
shadowMapFB->Unbind();

// Use shadow map in main pass
auto shadowTexture = shadowMapFB->GetDepthAttachment();
material->SetTexture("u_ShadowMap", shadowTexture);
```

## 💡 Example Usage

### Complete Game Application

```cpp
#include <SpoonEngine/SpoonEngine.h>

class MyGameApp : public SpoonEngine::Application {
public:
    MyGameApp() : Application("My Professional Game") {}
    
protected:
    void OnInitialize() override {
        // Set rendering clear color
        GetRenderer()->SetClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        
        auto world = GetWorld();
        
        // Register components
        world->RegisterComponent<SpoonEngine::Transform>();
        world->RegisterComponent<SpoonEngine::MeshRenderer>();
        world->RegisterComponent<SpoonEngine::CameraComponent>();
        
        // Set up rendering system
        m_RenderSystem = world->RegisterSystem<SpoonEngine::RenderSystem>();
        m_RenderSystem->SetWorld(world);
        
        SpoonEngine::Signature renderSignature;
        renderSignature.set(world->GetComponentManager()->GetComponentType<SpoonEngine::Transform>());
        renderSignature.set(world->GetComponentManager()->GetComponentType<SpoonEngine::MeshRenderer>());
        world->SetSystemSignature<SpoonEngine::RenderSystem>(renderSignature);
        
        // Create game objects
        CreatePlayer();
        CreateEnvironment();
        SetupCamera();
    }
    
    void OnUpdate(float deltaTime) override {
        // Handle input
        if (SpoonEngine::Input::IsKeyPressed(SpoonEngine::KeyCode::Escape)) {
            Shutdown();
        }
        
        // Update game logic
        UpdatePlayer(deltaTime);
        UpdateCamera(deltaTime);
    }
    
    void OnRender() override {
        // Rendering handled by RenderSystem automatically
        m_RenderSystem->Render(GetRenderer());
    }

private:
    std::shared_ptr<SpoonEngine::RenderSystem> m_RenderSystem;
    
    void CreatePlayer() {
        auto world = GetWorld();
        auto entity = world->CreateEntity();
        
        // Transform component
        SpoonEngine::Transform transform;
        transform.Position = SpoonEngine::Vector3(0, 0, 0);
        world->AddComponent(entity, transform);
        
        // Mesh renderer with material
        auto shader = SpoonEngine::Shader::Create("PlayerShader", vertexSrc, fragmentSrc);
        auto material = SpoonEngine::Material::Create(shader, "PlayerMaterial");
        auto mesh = SpoonEngine::Mesh::CreateCube();
        
        SpoonEngine::MeshRenderer renderer;
        renderer.Mesh = mesh;
        renderer.Material = material;
        world->AddComponent(entity, renderer);
    }
};

SPOON_MAIN(MyGameApp)
```

## 📈 Performance Characteristics

### **Rendering Performance**
- **Draw Calls**: Batched rendering with material state management
- **Shadow Maps**: 2048x2048 depth textures with efficient PCF filtering
- **Memory**: Component-based storage with cache-friendly data layout
- **GPU Utilization**: Modern OpenGL with VAO/VBO management

### **Engine Overhead**
- **ECS Performance**: O(1) component access, efficient iteration
- **Asset Loading**: Cached textures and meshes, minimal file I/O
- **System Coordination**: Event-driven updates, minimal CPU overhead

## 🤝 Contributing

SpoonEngine is designed as a professional learning project demonstrating modern game engine architecture. The codebase follows industry standards and is well-documented for educational purposes.

### **Code Standards**
- Modern C++17 features and best practices
- Clear separation of concerns and modular design
- Comprehensive documentation and examples
- Cross-platform compatibility considerations

### **Architecture Patterns**
- Entity Component System for game object management
- Factory pattern for asset creation
- Strategy pattern for renderer backends
- RAII for resource management

## 📄 License

MIT License - see LICENSE file for details.

---

## 🎯 Technical Achievements Summary

**SpoonEngine demonstrates professional game engine development with:**

✅ **24 Major Systems Implemented**  
✅ **Complete Asset Pipeline** (3D Model Loading, Shadow Mapping, Framebuffers, Materials, Scene Serialization)  
✅ **Production-Ready Architecture** (ECS, Asset Management, Cross-Platform)  
✅ **6 Professional Applications** (Basic → 3D → Textures → Shadows → Models → Editor)  
✅ **Modern C++ Implementation** (C++17, RAII, Smart Pointers)  
✅ **Industry-Standard Patterns** (Component-Based, Data-Oriented Design)

**Ready for portfolio presentation and open-source collaboration.**