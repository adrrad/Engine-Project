#include "core/EngineCore.hpp"
#include "utilities/Clock.hpp"
#include "core/GameObject.hpp"

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
    // INITIALISE SERIALISATION FOR DEFINED ENGINE OBJECTS
    GameObject().SerialiseProperties();
    Transform().SerialiseProperties();
    Scene().SerialiseProperties();
}

void EngineCore::InitialiseSubsystems()
{
    // INITIALIZE SUBSYSTEMS
    windowManager = Platform::WindowManager::GetInstance();
    // CREATE MAIN WINDOW
    mainWindow = windowManager->CreateWindow(settings.Project.ProjectName, settings.Window.Width, settings.Window.Height, settings.Window.Maximized);
    windowManager->SetActivewindow(mainWindow);
    renderer = Rendering::Renderer::GetInstance();
    physicsManager = Physics::PhysicsManager::GetInstance();
    componentManager = Components::ComponentManager::GetInstance();
    subsystems = {renderer, physicsManager, componentManager};
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
    renderer->RenderFrame();
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
        renderer->RenderFrame();
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
}

} // namespace Engine::Core