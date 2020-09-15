#pragma once
#include "core/Scene.hpp"
#include "platform/WindowManager.hpp"

namespace Engine::GUI
{

class SceneInspector
{
private:
    Platform::WindowManager* windowManager = nullptr;
    Core::Scene* _scene;
    Core::GameObject* _draggedGO = nullptr;
    Core::GameObject* _selectedGO = nullptr;
    WindowSize windowWidth, windowHeight;
    
    void DrawGameObjectNode(Engine::Core::GameObject* gameObject);

    void DrawSceneGraph();

    void DrawGameObjectInspector();

public:
    SceneInspector();

    void SetScene(Core::Scene* scene);

    void DrawGUI();
}; 

} // namespace GUI
