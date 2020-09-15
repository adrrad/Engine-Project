#include "core/EngineCore.hpp"
#include "utilities/Clock.hpp"

#include <chrono>

namespace Engine::Core
{

void EngineCore::Initialise()
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

EngineCore::EngineCore(EngineSettings settings)
{
    this->settings = settings;

    Initialise();
}

void EngineCore::GameLoop()
{
    float deltaTime = 0.0f;
    Utilities::Clock clock;
    componentManager->Start();
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

} // namespace Engine::Core