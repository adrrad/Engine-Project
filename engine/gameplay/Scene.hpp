#pragma once

#include "acceleration/Octree.hpp"
#include "Serialisable.hpp"
#include "EngineComponent.hpp"

#include <vector>
#include <string>

namespace Core
{
    class EngineCore;
}

namespace Gameplay
{
class GameObject;
class Transform;

class Scene : public Serialisable, public EngineComponent
{
friend class Core::EngineCore;
friend class Transform;

private:
    static Scene* MainScene;

    std::string m_name;

    std::vector<GameObject*> m_gameObjects;
    std::unordered_map<std::string, GameObject*> m_staticGameObjects;
    std::vector<GameObject*> m_dynamicGameObjects;

    Acceleration::Octree* m_staticTree = nullptr;

    Acceleration::Octree* m_dynamicTree = nullptr;

    void BuildStaticTree();

    void BuildDynamicTree();

    GameObject* FindOrInstantiateGameObject(Engine::GUID id);

public:
    Scene();

    Scene(std::string name);

    GameObject* InstantiateGameObject();

    void Update(float deltaTime);

    std::vector<GameObject*>& GetGameObjects();

    GameObject* FindGameObject(GameObjectID id);

    void SetStatic(GameObjectID id, bool isStatic);

    inline static Scene* GetMainScene() { return MainScene; }

    inline Acceleration::Octree* GetStaticTree() { return m_staticTree; }

    inline Acceleration::Octree* GetDynamicTree() { return m_dynamicTree; }

    std::shared_ptr<Utilities::JSON::JSONValue> Serialise() override;

    void Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json) override;
};


} // namespace Rendering
