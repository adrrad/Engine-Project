#pragma once
#include "core/Scene.hpp"


namespace Engine::GUI
{

class SceneInspector
{
private:
    Core::Scene* _scene;
    Core::GameObject* _selectedGO = nullptr;

    void DrawGameObjectNode(Engine::Core::GameObject* gameObject);

public:
    SceneInspector() = default;

    void SetScene(Core::Scene* scene);

    void DrawGUI();
}; 

} // namespace GUI
