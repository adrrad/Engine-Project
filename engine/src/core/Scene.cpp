#include "core/Scene.hpp"
#include "components/ComponentManager.hpp"
#include "platform/GUID.hpp"
#include "core/GameObject.hpp"

namespace Engine::Core
{

Scene* Scene::MainScene = nullptr;

void Scene::BuildStaticTree()
{
    std::vector<GameObject*> staticObjects;
    for(auto pair : m_staticGameObjects) staticObjects.push_back(pair.second);
    m_staticTree = new Acceleration::Octree(staticObjects, 4);
}

void Scene::BuildDynamicTree()
{
    m_dynamicTree = new Acceleration::Octree(m_dynamicGameObjects, 4);
}

GameObject* Scene::FindOrInstantiateGameObject(GUID id)
{
    GameObject* go = FindGameObject(id);
    if(go == nullptr)
    {
        go = InstantiateGameObject();
        go->ID = id;
    }
    return go;
}

Scene::Scene()
{
    MainScene = this;
}

GameObject* Scene::InstantiateGameObject()
{
    GameObjectID id = Platform::GenerateGUID();
    GameObject* go = new GameObject(id, this);
    m_dynamicGameObjects.push_back(go);
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

GameObject* Scene::FindGameObject(GameObjectID id)
{
    std::string guid = id.ToString();
    if(m_staticGameObjects.contains(guid)) return m_staticGameObjects[guid];
    for(auto go : m_dynamicGameObjects) if(go->ID == id) return go;
    return nullptr;
}

void Scene::SetStatic(GameObjectID id, bool isStatic)
{
    std::string guid = id.ToString();
    GameObject* go = FindGameObject(id);
    if(isStatic)
    {
        m_dynamicGameObjects.erase(std::find(m_dynamicGameObjects.begin(), m_dynamicGameObjects.end(), go));
        m_staticGameObjects.insert({guid, go});
    }
    else
    {
        m_staticGameObjects.erase(guid);
        m_dynamicGameObjects.push_back(go);
    }
    // Mark children static as well
    for(auto child : go->transform.GetChildren()) { SetStatic(child->gameObject->ID, isStatic); }
}

std::shared_ptr<Utilities::JSON::JSONValue> Scene::Serialise()
{
    using namespace Utilities::JSON;

    auto serialiseGameObjects = [&]()
    {
        std::vector<std::shared_ptr<JSONValue>> gos;
        for(auto go : m_gameObjects) gos.push_back(go->Serialise());
        return JSONValue::AsArray(gos);
    };

    return JSONValue::AsObject({
        { "gameObjects", serialiseGameObjects() }
    });
}

void Scene::Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json)
{
    auto serialisedObjects = json->GetMember("gameObjects")->Array;
    for(auto sgo : serialisedObjects)
    {
        GameObjectID id = sgo->GetMember("id")->String;
        GameObject* go = FindOrInstantiateGameObject(id);
        go->Deserialise(sgo);
    }
}

} // namespace Engine::Rendering
