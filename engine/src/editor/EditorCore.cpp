#include "editor/EditorCore.hpp"
#include "utilities/Clock.hpp"

namespace Engine::Editor
{

void EditorCore::Initialise()
{
    // INITIALIZE SUBSYSTEMS
    windowManager = Platform::WindowManager::GetInstance();
    // CREATE MAIN WINDOW
    mainWindow = windowManager->CreateWindow(settings.Project.ProjectName, settings.Window.Width, settings.Window.Height, settings.Window.Maximized);
    windowManager->SetActivewindow(mainWindow);
    renderer = Rendering::Renderer::GetInstance();
}

bool EditorCore::ShouldClose()
{
    return windowManager->WindowShouldClose(mainWindow);
}

void EditorCore::Close()
{

}

EditorCore::EditorCore(Core::EngineSettings settings)
{
    this->settings = settings;
}

void EditorCore::SetEditMode()
{
    currentMode = EditorMode::EDIT;
}

void EditorCore::SetPlayMode()
{
    currentMode = EditorMode::PLAY;
}

void EditorCore::SetPauseMode()
{
    currentMode = EditorMode::PAUSE;
}


void EditorCore::EditorLoop()
{
    float deltaTime = 0.0f;
    Utilities::Clock clock;
    while(!ShouldClose())
    {
        // RENDER THE FRAME AND MEASURE THE FRAME TIME
        clock.Start();
        renderer->RenderFrame();
        windowManager->SwapBuffers(mainWindow);
        deltaTime = clock.Stop();
        windowManager->PollEvents();
    }
}

}