#include "core/Scene.hpp"

#include "components/ComponentManager.hpp"

namespace Engine::Core
{

GameObject* Scene::InstantiateGameObject()
{
    GameObject* go = new GameObject();
    m_gameObjects.push_back(go);
    return go;
}

void Scene::AddGameObject(GameObject *object)
{
    m_gameObjects.push_back(object);
}
    
void Scene::Update(float deltaTime)
{
   Components::ComponentManager::GetInstance()->Update(deltaTime);
}

std::vector<GameObject*>& Scene::GetGameObjects()
{
    return m_gameObjects;
}

} // namespace Engine::Rendering
