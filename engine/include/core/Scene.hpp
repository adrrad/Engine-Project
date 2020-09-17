#pragma once
#include "core/GameObject.hpp"

#include <vector>

namespace Engine::Core
{
    
class Scene
{
private:
    std::vector<GameObject*> m_gameObjects;

public:
    GameObject* InstantiateGameObject();
    
    void AddGameObject(GameObject *object);

    void Update(float deltaTime);

    std::vector<GameObject*>& GetGameObjects();
};

} // namespace Engine::Rendering
