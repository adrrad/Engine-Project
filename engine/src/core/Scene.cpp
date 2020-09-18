#include "core/Scene.hpp"

#include "components/ComponentManager.hpp"

namespace Engine::Utilities::Serialisation
{
    template<class C>
    void SerialiseProperty(C* object, const std::string& name, const std::vector<Core::GameObject*>& value)
    {
        std::string typeName = typeid(C).name();
        if(IsSerialised(typeName, name)) return;
        __int64 offset = (char*)&value - (char*)object;
        auto serializer = [offset, name](void* objPtr, int indent){
            std::vector<Core::GameObject*>* varloc = (std::vector<Core::GameObject*>*)((char*)objPtr+offset);
            std::vector<Core::GameObject*>& vals = *varloc;
            std::vector<std::string> gos;
            for(auto go : vals)
            {
                gos.push_back(SerializeObject(go, false, indent));
            }
            return KeyValuePair("Game Objects", JSONArray(gos), indent);
        };
        AddSerializer(typeName,serializer);
        Serialiser::SerializedProps.insert(typeName+name);
    }
}

namespace Engine::Core
{

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

void Scene::SerialiseProperties()
{
    SERIALISE_PROPERTY(m_gameObjects);
}

} // namespace Engine::Rendering
