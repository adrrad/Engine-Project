#include "core/EngineCore.hpp"
#include "core/Clock.hpp"

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
    Clock clock;
    while(!ShouldClose())
    {
        // RENDER THE FRAME AND MEASURE THE FRAME TIME
        clock.Start();
        renderer->RenderFrame();

        // SWAP BUFFERS
        windowManager->SwapBuffers(mainWindow);
        windowManager->PollEvents();
        deltaTime = clock.Stop();
        // UPDATE THE PHYSICS SUBSYSTEM
        physicsManager->Update(deltaTime);

        // UPDATE THE COMPONENTS
        componentManager->Update(deltaTime);
    }
}

} // namespace Engine::Core