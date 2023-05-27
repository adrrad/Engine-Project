#include "core/EngineCore.hpp"
#include "utilities/Clock.hpp"
#include "gameplay/GameObject.hpp"
#include "gameplay/Scene.hpp"
#include "memory/Allocator.hpp"
#include "components/StandardInclude.hpp"
#include "EngineComponent.hpp"

#include "utilities/MathUtils.hpp"

#include <chrono>

using namespace Utilities;

namespace Core
{

void EngineCore::InitialiseEngine()
{
    InitialiseSettings();
    InitialiseUtilities();
    InitialiseSubsystems();
}

void EngineCore::InitialiseSettings()
{
    EngineSettings& settings = EngineSettings::CreateEngineSettingsInstance();
    settings.Project.ProjectName = "Engine";
    settings.Project.RootDirectory = RESOURCES_DIR;
    settings.Window.Width = 1920;
    settings.Window.Height = 1080;
    settings.Window.Maximized = false;

    // Memory
    settings.Memory.TotalMemory = GB2B(1U) + MB2B(250U);
    settings.Memory.AssetsMemory = MB2B(200U);
    settings.Memory.PhysicsMemory = MB2B(100U);
    settings.Memory.RendererMemory = MB2B(200U);
    settings.Memory.UtilitiesMemory = MB2B(100U);
    settings.Memory.SceneMemory = MB2B(50U);
    settings.Memory.ComponentMemory = MB2B(50U);
    settings.Memory.GameplayMemory = settings.Memory.SceneMemory + settings.Memory.ComponentMemory + MB2B(50U);
}

void EngineCore::InitialiseUtilities()
{

}


void EngineCore::InitialiseSubsystems()
{
    EngineSettings settings = EngineSettings::Get();
    // INITIALIZE SUBSYSTEMS
    memoryManager = InitialiseSubsystem<Memory::MemoryManager>();
    memoryManager->AllocateMemory(settings.Memory.TotalMemory);

    windowManager = InitialiseSubsystem<Platform::WindowManager>();
    // CREATE MAIN WINDOW
    mainWindow = windowManager->CreateMainWindow();
    windowManager->SetActivewindow(mainWindow);
    assetManager = InitialiseSubsystem<Assets::AssetManager>();
    // renderer = InitialiseSubsystem<Rendering::Renderer>();
    physicsManager = InitialiseSubsystem<Physics::PhysicsManager>();
    sceneManager = InitialiseSubsystem<Gameplay::SceneManager>();
    gameplayManager = InitialiseSubsystem<Gameplay::GameplayManager>();
    renderer = InitialiseSubsystem<Rendering::Renderer>();
    hlrenderer = InitialiseSubsystem<Rendering::HLRenderer>();
}

void EngineCore::UpdateSubsystems(float deltaTime)
{
    // for(auto system : subsystems) system->Update(deltaTime);
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
    // renderer->RecordScene(Gameplay::Scene::GetMainScene());
    
    hlrenderer->DrawFrame();
    // renderer->RenderFrame();
    windowManager->SwapBuffers(mainWindow);
    float deltaTime = clock.Stop();
    windowManager->PollEvents();
    // UPDATE THE PHYSICS SUBSYSTEM
    physicsManager->Update(deltaTime);
    // UPDATE THE COMPONENTS
    gameplayManager->Update(deltaTime);
}

EngineCore::EngineCore()
{
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
        // renderer->RecordScene(Gameplay::Scene::GetMainScene());
        
        hlrenderer->DrawFrame();
        // renderer->RenderFrame();
        windowManager->SwapBuffers(mainWindow);
        deltaTime = clock.Stop();
        windowManager->PollEvents();
        // UPDATE THE PHYSICS SUBSYSTEM
        physicsManager->Update(deltaTime);
        // UPDATE THE COMPONENTS
        gameplayManager->Update(deltaTime);
    }
}

void EngineCore::Start()
{
    gameplayManager->Start();
    Gameplay::Scene::GetMainScene()->BuildStaticTree();
    Gameplay::Scene::GetMainScene()->BuildDynamicTree();
}

} // namespace Core