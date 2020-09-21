#pragma once
#include "core/GameObject.hpp"
#include "utilities/serialisation/Serialisation.hpp"
#include <vector>

namespace Engine::Core
{
    
class Scene : public Utilities::Serialisation::Serialisable
{
private:
    SERIALISABLE(Scene, std::vector<GameObject*>, m_gameObjects);

    SERIALISABLE(Scene, std::vector<Components::IComponentPool*>*, m_componentPools);

public:
    Scene();

    GameObject* InstantiateGameObject();
    
    void AddGameObject(GameObject *object);

    void Update(float deltaTime);

    std::vector<GameObject*>& GetGameObjects();

    inline std::string GetSerialised(int indent) override;
    
};

std::string Scene::GetSerialised(int indent)
{
    return Utilities::Serialisation::SerializeObject(this, indent);
}

} // namespace Engine::Rendering
