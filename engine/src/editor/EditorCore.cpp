#include "editor/EditorCore.hpp"
#include "utilities/Clock.hpp"

namespace Engine::Editor
{

void EditorCore::Initialise()
{
    engineCore = new Core::EngineCore(settings);
    // Get WindowManager & Renderer references. Window is created by the EngineCore object.
    windowManager = Platform::WindowManager::GetInstance();
    mainWindow = windowManager->GetActiveWindow();
    renderer = Rendering::Renderer::GetInstance();
    // Register GUI callbacks and GUI draw function.
    sceneInspector.SetPlayCallback([&]() { SetPlayMode(); });
    sceneInspector.SetPauseCallback([&]() { SetPauseMode(); });
    sceneInspector.SetStopCallback([&]() { SetEditMode(); });
    renderer->RegisterGUIDraw([&]() { this->sceneInspector.DrawGUI(); });
}

bool EditorCore::ShouldClose()
{
    return windowManager->WindowShouldClose(mainWindow);
}

void EditorCore::Close()
{

}

void EditorCore::EditorLoop()
{
    float deltaTime = 0.0f;
    Utilities::Clock clock;
    while(!ShouldClose() && currentMode == EditorMode::EDIT)
    {
        // RENDER THE FRAME AND MEASURE THE FRAME TIME
        clock.Start();
        renderer->RenderFrame();
        windowManager->SwapBuffers(mainWindow);
        deltaTime = clock.Stop();
        windowManager->PollEvents();
    }
}

void EditorCore::PlayLoop()
{
    float deltaTime = 0.0f;
    Utilities::Clock clock;
    while(!ShouldClose() && currentMode == EditorMode::PLAY)
    {
        engineCore->LoopIteration();
    }
}

EditorCore::EditorCore(Core::EngineSettings settings)
{
    this->settings = settings;
    Initialise();
}


void EditorCore::Run()
{
    // While the program is open, run in one of the specified editor modes.
    while(!ShouldClose())
    {
        switch (currentMode)
        {
        case EditorMode::EDIT:
            EditorLoop();
            break;
        case EditorMode::PLAY:
            PlayLoop();
            break;
        case EditorMode::PAUSE:
            //TODO: implement this
            throw std::exception("Pause mode not implemented yet");
            break;
        
        default:
            throw std::exception("Unknown editor mode!");
            break;
        }
    }
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

void EditorCore::SetCurrentScene(Core::Scene* scene)
{
    currentScene = scene;
    sceneInspector.SetScene(scene);
}

}