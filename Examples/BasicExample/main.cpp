#include <SpoonEngine/SpoonEngine.h>
#include <iostream>

class BasicExampleApp : public SpoonEngine::Application {
public:
    BasicExampleApp() : Application("SpoonEngine - Basic Example") {
        std::cout << "BasicExampleApp created" << std::endl;
    }
    
    virtual ~BasicExampleApp() {
        std::cout << "BasicExampleApp destroyed" << std::endl;
    }
    
protected:
    void OnInitialize() override {
        std::cout << "BasicExampleApp initialized" << std::endl;
        
        GetRenderer()->SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        
        auto world = GetWorld();
        
        world->RegisterComponent<SpoonEngine::Vector3>();
        
        auto entity = world->CreateEntity();
        world->AddComponent<SpoonEngine::Vector3>(entity, SpoonEngine::Vector3(1.0f, 2.0f, 3.0f));
        
        std::cout << "Entity created with position component" << std::endl;
    }
    
    void OnUpdate(float deltaTime) override {
        if (SpoonEngine::Input::IsKeyPressed(SpoonEngine::KeyCode::Escape)) {
            Shutdown();
        }
    }
    
    void OnRender() override {
    }
    
    void OnShutdown() override {
        std::cout << "BasicExampleApp shutdown" << std::endl;
    }
};

SPOON_MAIN(BasicExampleApp)