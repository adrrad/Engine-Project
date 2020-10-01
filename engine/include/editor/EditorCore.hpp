#pragma once
#include "EngineTypedefs.hpp"
#include "editor/SceneInspector.hpp"
#include "core/EngineCore.hpp"
#include "core/EngineSettings.hpp"
#include "core/Scene.hpp"
#include "rendering/Renderer.hpp"
#include "physics/PhysicsManager.hpp"
#include "components/ComponentManager.hpp"
#include <vector>


namespace Engine::Editor
{


class EditorCore
{
enum class EditorMode { EDIT, PLAY, PAUSE };
private:
    WindowHandle mainWindow;
    Core::EngineSettings settings;
    Platform::WindowManager* windowManager;
    Rendering::Renderer* renderer;
    Core::EngineCore* engineCore;
    Core::Scene* currentScene;
    SceneInspector* sceneInspector = nullptr;
    EditorMode currentMode = EditorMode::EDIT;
    
    std::vector<Components::BaseComponent*> editorObjectComponents;
    Core::GameObject editorCamera;

    void Initialise();

    void InitialiseCameraObject();

    bool ShouldClose();

    void Close();

    void EditorLoop();

    void PlayLoop();
public:

    EditorCore(Core::EngineSettings settings);

    void Run();

    void SetEditMode();

    void SetPlayMode();

    void SetPauseMode();

    void SetCurrentScene(Core::Scene* scene);
};

} // namespace Engine::GUI
