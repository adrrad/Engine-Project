#include "renderer/Scene.hpp"

#include "components/ComponentPool.hpp"

namespace Rendering
{


void Scene::AddGameObject(Engine::Core::GameObject *object)
{
    _GameObjects.push_back(object);
}
    
void Scene::Update(float deltaTime)
{
   Components::ComponentManager::UpdateAllComponents(deltaTime);
}

std::vector<Engine::Core::GameObject*>& Scene::GetGameObjects()
{
    return _GameObjects;
}

} // namespace Rendering
