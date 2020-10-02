#include "core/Scene.hpp"
#include "components/ComponentManager.hpp"
#include "platform/GUID.hpp"
#include "core/GameObject.hpp"

namespace Engine::Core
{

void Scene::BuildStaticTree()
{

}

void Scene::BuildDynamicTree()
{

}

Scene::Scene()
{
    
}

GameObject* Scene::InstantiateGameObject()
{
    GameObjectID id = Platform::GenerateGUID();
    GameObject* go = new GameObject(id, this);
    m_dynamicGameObjects.insert({id.ToString(), go});
    m_gameObjects.push_back(go);
    return go;
}
    
void Scene::Update(float deltaTime)
{
   Components::ComponentManager::GetInstance()->Update(deltaTime);
}

std::vector<GameObject*>& Scene::GetGameObjects()
{
    return m_gameObjects;
}

GameObject* Scene::GetGameObject(GameObjectID id)
{
    std::string guid = id.ToString();
    if(m_dynamicGameObjects.contains(guid)) return m_dynamicGameObjects[guid];
    if(m_staticGameObjects.contains(guid)) return m_staticGameObjects[guid];
    return nullptr;
}

void Scene::SetStatic(GameObjectID id, bool isStatic)
{
    std::string guid = id.ToString();
    if(isStatic)
    {
        GameObject* go = m_dynamicGameObjects.at(guid);
        m_dynamicGameObjects.erase(guid);
        m_staticGameObjects.insert({guid, go});
    }
    else
    {
        GameObject* go = m_staticGameObjects.at(guid);
        m_staticGameObjects.erase(guid);
        m_dynamicGameObjects.insert({guid, go});
    }
}

} // namespace Engine::Rendering
