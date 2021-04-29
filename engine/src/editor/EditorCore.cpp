#include "editor/EditorCore.hpp"
#include "utilities/Clock.hpp"
#include "core/Scene.hpp"

#include "components/CameraComponent.hpp"
#include "components/InspectorCameraComponent.hpp"
#include "components/LightComponent.hpp"

#include "assets/resources/CubemapAsset.hpp"

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
    sceneInspector = new SceneInspector();
    sceneInspector->SetPlayCallback([&]() { SetPlayMode(); });
    sceneInspector->SetPauseCallback([&]() { SetPauseMode(); });
    sceneInspector->SetStopCallback([&]() { SetEditMode(); });
    renderer->RegisterGUIDraw([&]() { this->sceneInspector->DrawGUI(); });
    InitialiseCameraObject();
}

void EditorCore::InitialiseCameraObject()
{
    editorCamera.Name = "Editor Camera";
    auto cam = editorCamera.AddComponent<Components::CameraComponent>();
    auto mov = editorCamera.AddComponent<Components::InspectorCameraComponent>();
    cam->Start();
    mov->Start();
    // cam->SetSkybox(Assets::AssetManager::GetInstance()->GetAsset<Assets::CubemapAsset>("skybox.cubemap"));
    editorObjectComponents.push_back(cam);
    editorObjectComponents.push_back(mov);
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
    editorCamera.SetEnabled(true);
    editorCamera.GetComponent<Components::CameraComponent>()->SetMain();
    Core::Scene::GetMainScene()->BuildStaticTree();
    Core::Scene::GetMainScene()->BuildDynamicTree();
    while(!ShouldClose() && currentMode == EditorMode::EDIT)
    {
        // RENDER THE FRAME AND MEASURE THE FRAME TIME
        clock.Start();
        renderer->RecordScene(Core::Scene::GetMainScene());
        renderer->RenderFrame();
        
        windowManager->SwapBuffers(mainWindow);
        deltaTime = clock.Stop();
        windowManager->PollEvents();
        for(auto comp : editorObjectComponents) comp->Update(deltaTime);
        Components::ComponentManager::GetComponentPool<Components::LightComponent>()->Update(deltaTime);
    }
}

void EditorCore::PlayLoop()
{
    float deltaTime = 0.0f;
    Utilities::Clock clock;
    engineCore->Start();
    editorCamera.SetEnabled(false);
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
    sceneInspector->SetScene(scene);
}

}