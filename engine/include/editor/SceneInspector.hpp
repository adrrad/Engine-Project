#pragma once
#include "core/Scene.hpp"
#include "assets/AssetManager.hpp"
#include "platform/WindowManager.hpp"
#include "platform/io/Directory.hpp"

#include "editor/gui/Panel.hpp"

#include <functional>

namespace Engine::Editor
{

class SceneInspector
{
private:
    Assets::AssetManager* m_assetManager = nullptr;
    Platform::WindowManager* m_windowManager = nullptr;
    Core::Scene* m_scene;
    Core::GameObject* m_draggedGO = nullptr;
    Core::GameObject* m_selectedGO = nullptr;

    Panel topPanel = Panel("Controls", 0, 0, 0, 0,
        PanelPlacement::AUTO, PanelAlignment::TOPLEFT);

    Panel leftPanel = Panel("Overview", 0, 0, 0, GUIProperties::WindowHeight,
        PanelPlacement::AUTO, PanelAlignment::TOPLEFT);

    Panel rightPanel = Panel("Inspector", 0, 0, 0, GUIProperties::WindowHeight,
        PanelPlacement::AUTO, PanelAlignment::RIGHT);

    std::function<void()> playCallback, pauseCallback, stopCallback;

    void DrawGameObjectNode(Engine::Core::GameObject* gameObject);

    void DrawSceneGraph();

    void DrawGameObjectInspector();

    void DrawControlPanel();

    void DrawDirectoryContent(Platform::IO::Directory* dir);

    void DrawProjectFiles();

    void DrawRightPanel();

    void DrawLeftPanel();

    void DrawTopPanel();



public:
    SceneInspector();

    void SetScene(Core::Scene* scene);

    void DrawGUI();

    void SetPlayCallback(std::function<void()> callback);

    void SetPauseCallback(std::function<void()> callback);
    
    void SetStopCallback(std::function<void()> callback);
}; 

} // namespace GUI
