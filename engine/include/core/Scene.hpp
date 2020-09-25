#pragma once
#include "core/GameObject.hpp"
#include "utilities/serialisation/Serialisation.hpp"
#include <vector>

namespace Engine::Core
{
    
class Scene : public Utilities::Serialisation::Serialisable<Scene>
{
private:
    // SERIALISABLE(Scene, std::vector<Components::IComponentPool*>*, m_componentPools);
    SERIALISABLE(Scene, std::vector<GameObject*>, m_gameObjects);


public:
    Scene();

    GameObject* InstantiateGameObject();
    
    void AddGameObject(GameObject *object);

    void Update(float deltaTime);

    std::vector<GameObject*>& GetGameObjects();

    inline GameObject* GetGameObject(GameObjectID id)
    {
        for(auto go : m_gameObjects) if(go->ID == id) return go;
        return nullptr;
    }

};


} // namespace Engine::Rendering
