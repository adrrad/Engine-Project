#include "core/EngineCore.hpp"
#include "utilities/Clock.hpp"
#include "core/GameObject.hpp"
#include "core/Scene.hpp"

#include "components/StandardInclude.hpp"

#include <chrono>


namespace Engine::Core
{

void EngineCore::InitialiseEngine()
{
    InitialiseUtilities();
    InitialiseSubsystems();
}

void EngineCore::InitialiseUtilities()
{

}

void EngineCore::InitialiseComponentPools()
{
    componentManager = Components::ComponentManager::GetInstance();
    componentManager->RegisterComponentPool<Components::CameraComponent>();
    componentManager->RegisterComponentPool<Components::InspectorCameraComponent>();
    componentManager->RegisterComponentPool<Components::MeshComponent>();
    componentManager->RegisterComponentPool<Components::RigidBodyComponent>();
    componentManager->RegisterComponentPool<Components::LightComponent>();
    componentManager->RegisterComponentPool<Components::MovementComponent>();
}

void EngineCore::InitialiseSubsystems()
{
    // INITIALIZE SUBSYSTEMS
    windowManager = Platform::WindowManager::GetInstance();
    // CREATE MAIN WINDOW
    mainWindow = windowManager->CreateWindow(settings.Project.ProjectName, settings.Window.Width, settings.Window.Height, settings.Window.Maximized);
    windowManager->SetActivewindow(mainWindow);
    renderer = Rendering::Renderer::GetInstance();
    sceneManager = new SceneManager();
    hlrenderer = new HLRendering::HLRenderer(renderer, sceneManager);
    physicsManager = Physics::PhysicsManager::GetInstance();
    InitialiseComponentPools();
    assetManager = new Assets::AssetManager(settings.Project.RootDirectory);
    subsystems = { renderer, physicsManager, componentManager };
}

void EngineCore::UpdateSubsystems(float deltaTime)
{
    for(auto system : subsystems) system->Update(deltaTime);
}

bool EngineCore::ShouldClose()
{
    return windowManager->WindowShouldClose(mainWindow);
}

void EngineCore::Close()
{
    throw "What to do?";
}

void EngineCore::LoopIteration()
{
    Utilities::Clock clock;
    // RENDER THE FRAME AND MEASURE THE FRAME TIME
    clock.Start();
    // renderer->RecordScene(Scene::GetMainScene());
    // renderer->RenderFrame();
    hlrenderer->DrawFrame();
    windowManager->SwapBuffers(mainWindow);
    float deltaTime = clock.Stop();
    windowManager->PollEvents();
    // UPDATE THE PHYSICS SUBSYSTEM
    physicsManager->Update(deltaTime);
    // UPDATE THE COMPONENTS
    componentManager->Update(deltaTime);
}

EngineCore::EngineCore(EngineSettings settings)
{
    this->settings = settings;
    InitialiseEngine();
}

void EngineCore::GameLoop()
{
    float deltaTime = 0.0f;
    Utilities::Clock clock;
    Start();
    while(!ShouldClose())
    {
        // RENDER THE FRAME AND MEASURE THE FRAME TIME
        clock.Start();
        renderer->RecordScene(Scene::GetMainScene());
        renderer->RenderFrame();
        // hlrenderer->DrawFrame();
        windowManager->SwapBuffers(mainWindow);
        deltaTime = clock.Stop();
        windowManager->PollEvents();
        // UPDATE THE PHYSICS SUBSYSTEM
        physicsManager->Update(deltaTime);
        // UPDATE THE COMPONENTS
        componentManager->Update(deltaTime);
    }
}

void EngineCore::Start()
{
    componentManager->Start();
    Scene::GetMainScene()->BuildStaticTree();
    Scene::GetMainScene()->BuildDynamicTree();
}

} // namespace Engine::Core