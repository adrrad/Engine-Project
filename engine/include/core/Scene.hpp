#pragma once
#include "core/GameObject.hpp"
#include "utilities/serialisation/Serialisation.hpp"
#include <vector>

namespace Engine::Core
{
    
class Scene : public Utilities::Serialisation::Serialisable
{
private:
    std::vector<GameObject*> m_gameObjects;

public:
    GameObject* InstantiateGameObject();
    
    void AddGameObject(GameObject *object);

    void Update(float deltaTime);

    std::vector<GameObject*>& GetGameObjects();

    void SerialiseProperties();
};

} // namespace Engine::Rendering
