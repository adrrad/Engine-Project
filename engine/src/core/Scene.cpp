#include "core/Scene.hpp"

#include "components/ComponentManager.hpp"

namespace Engine::Utilities::Serialisation
{
    template<class C>
    std::vector<Core::GameObject*> SerialiseProperty(Offset offset, std::string memberName, std::vector<Core::GameObject*>& value)
    {
        std::string typeName = typeid(C).name();
        if(!Serialiser::IsSerialised<C>(memberName))
        {
            auto serializer = [offset, memberName](void* objPtr, int indent){
                std::vector<Core::GameObject*>* varloc = (std::vector<Core::GameObject*>*)((char*)objPtr+offset);
                std::vector<Core::GameObject*>& vals = *varloc;
                std::vector<std::string> gos;
                for(auto go : vals)
                {
                    gos.push_back(SerializeObject(go, indent));
                }
                return KeyValuePair("Game Objects", JSONArray(gos), indent);
            };
            AddSerializer<C>(memberName,serializer);
        }
        return std::vector<Core::GameObject*>();
    }

    template<class C>
    std::vector<Components::IComponentPool*>* SerialiseProperty(Offset offset, std::string memberName, std::vector<Components::IComponentPool*>*& value)
    {
        if(!Serialiser::IsSerialised<C>(memberName))
        {
            auto serializer = [offset, memberName](void* objPtr, int indent){
                std::vector<Components::IComponentPool*>** varloc = (std::vector<Components::IComponentPool*>**)((char*)objPtr+offset);
                std::vector<Components::IComponentPool*>* vals = *varloc;
                std::vector<std::string> pools;
                for(auto pool : *vals)
                {
                    pools.push_back(KeyValuePairObject(pool->GetName(), pool->GetSerialised(indent), indent));
                }
                return KeyValuePair("Component Pools", JSONArray(pools), indent);
            };
            AddSerializer<C>(memberName,serializer);
        }
        return value;
    }
}

namespace Engine::Core
{

Scene::Scene()
{
    m_componentPools = &Components::ComponentManager::GetAllPools();
}

GameObject* Scene::InstantiateGameObject()
{
    GameObjectID goID = GameObjectID(m_gameObjects.size());
    GameObject* go = new GameObject(goID);
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
