#pragma once
#include "core/GameObject.hpp"

#include <vector>

namespace Rendering
{
    
class Scene
{
private:
    std::vector<Engine::Core::GameObject*> _GameObjects;

public:
    void AddGameObject(Engine::Core::GameObject *object);
    void Update(float deltaTime);

    std::vector<Engine::Core::GameObject*>& GetGameObjects();
};

} // namespace Rendering
