#include "core/Scene.hpp"

#include "components/ComponentPool.hpp"

namespace Engine::Core
{


void Scene::AddGameObject(GameObject *object)
{
    _GameObjects.push_back(object);
}
    
void Scene::Update(float deltaTime)
{
   Components::ComponentManager::GetInstance()->Update(deltaTime);
}

std::vector<GameObject*>& Scene::GetGameObjects()
{
    return _GameObjects;
}

} // namespace Rendering
