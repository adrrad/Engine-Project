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
    Assets::AssetManager* assetManager = nullptr;
    Platform::WindowManager* windowManager = nullptr;
    Core::Scene* _scene;
    Core::GameObject* _draggedGO = nullptr;
    Core::GameObject* _selectedGO = nullptr;
    WindowSize windowWidth, windowHeight;
    
    glm::vec2 SGwindowPos, OIwindowPos;
    glm::vec2 SGwindowSize, OIwindowSize;

    Panel topPanel = Panel("Top Panel", 0, 0, 0, 0,
        PanelPlacement::AUTO, PanelAlignment::TOPLEFT);

    Panel leftPanel = Panel("Left Panel", 0, 0, 0, GUIProperties::WindowHeight,
        PanelPlacement::AUTO, PanelAlignment::TOPLEFT);

    Panel rightPanel = Panel("Right Panel", 0, 0, 0, GUIProperties::WindowHeight,
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
