#pragma once
#include "core/Scene.hpp"
#include "platform/WindowManager.hpp"



#include <functional>

namespace Engine::Editor
{

class SceneInspector
{
private:
    Platform::WindowManager* windowManager = nullptr;
    Core::Scene* _scene;
    Core::GameObject* _draggedGO = nullptr;
    Core::GameObject* _selectedGO = nullptr;
    WindowSize windowWidth, windowHeight;
    
    glm::vec2 SGwindowPos, OIwindowPos;
    glm::vec2 SGwindowSize, OIwindowSize;

    std::function<void()> playCallback, pauseCallback, stopCallback;

    void DrawGameObjectNode(Engine::Core::GameObject* gameObject);

    void DrawSceneGraph();

    void DrawGameObjectInspector();

    void DrawControlPanel();

public:
    SceneInspector();

    void SetScene(Core::Scene* scene);

    void DrawGUI();

    void SetPlayCallback(std::function<void()> callback);

    void SetPauseCallback(std::function<void()> callback);
    
    void SetStopCallback(std::function<void()> callback);
}; 

} // namespace GUI
