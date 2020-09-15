#pragma once
#include "EngineTypedefs.hpp"
#include "core/EngineCore.hpp"
#include "core/EngineSettings.hpp"
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


    EditorMode currentMode = EditorMode::EDIT;

    void Initialise();

    bool ShouldClose();

    void Close();

public:

    EditorCore(Core::EngineSettings settings);

    void SetEditMode();

    void SetPlayMode();

    void SetPauseMode();

    void EditorLoop();

};

} // namespace Engine::GUI
