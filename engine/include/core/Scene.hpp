#pragma once
#include "utilities/serialisation/Serialisation.hpp"
#include "acceleration/Octree.hpp"
#include <vector>

namespace Engine::Core
{
class GameObject;
class Scene : public Utilities::Serialisation::Serialisable<Scene>
{
friend class Engine::Rendering::Renderer;

private:
    SERIALISABLE(Scene, std::vector<GameObject*>, m_gameObjects);
    std::unordered_map<std::string, GameObject*> m_staticGameObjects;
    std::vector<GameObject*> m_dynamicGameObjects;

    Acceleration::Octree* m_staticTree = nullptr;

    Acceleration::Octree* m_dynamicTree = nullptr;

    void BuildStaticTree();

    void BuildDynamicTree();

    __forceinline Acceleration::Octree* GetStaticTree() { return m_staticTree; }

    __forceinline Acceleration::Octree* GetDynamicTree() { return m_dynamicTree; }

public:
    Scene();

    GameObject* InstantiateGameObject();

    void Update(float deltaTime);

    std::vector<GameObject*>& GetGameObjects();

    GameObject* GetGameObject(GameObjectID id);

    void SetStatic(GameObjectID id, bool isStatic);

};


} // namespace Engine::Rendering
