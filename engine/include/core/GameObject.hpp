#pragma once
#include "core/Transform.hpp"
#include "core/Scene.hpp"
#include "components/BaseComponent.hpp"
#include "components/ComponentManager.hpp"
#include "geometry/Volume.hpp"
#include "Serialisable.hpp"

#include <vector>
#include <unordered_map>
#include <string>

namespace Engine::Editor
{
    class SceneInspector;
    class EditorCore;
}

namespace Engine::Core
{
//FORWARD DECLARATIONS
class EngineCore;

class GameObject : public Serialisable
{
friend class Engine::Editor::SceneInspector;
friend class Engine::Core::EngineCore;
friend class Engine::Core::Scene;
friend class Engine::Editor::EditorCore;
private:
    GameObjectID ID;

    Scene* m_scene;
    
    std::unordered_map<std::string, Components::BaseComponent*> m_components;
    
    bool m_static;

    bool m_enabled;


    __forceinline const std::unordered_map<std::string, Components::BaseComponent*> GetComponents();

    /**
     * @brief The appropriate constructor for GameObjects used in the final game.
     * 
     * @param id The game object ID. Defined by the instantiating scene object.
     */
    GameObject(GameObjectID id, Scene* scene);

    /**
     * @brief Constructor for a GameObject used by the editor or other utilities.
     * 
     */
    GameObject();

public:

    Transform transform;
    std::string Name;


    __forceinline GameObjectID GetID();

    __forceinline bool Enabled();

    __forceinline void SetEnabled(bool isEnabled);

    __forceinline bool Static();

    __forceinline void SetStatic(bool isStatic);
    
    /**
     * @brief Adds a component specified by type. The type must derive from BaseComponent.
     * 
     * @tparam T The component class type.
     * @return T* A pointer to the instantiated component object.
     */
    template <class T>
    T* AddComponent();

    /**
     * @brief Retrieves a component specified by a type. The type must derived from BaseComponent.
     * 
     * @tparam T The component class type.
     * @return T* A pointer to the component object, or nullptr if the object has none attached.
     */
    template <class T>
    T* GetComponent();

    /**
     * @brief TODO: Check if needed at all
     * 
     * @param deltaTime 
     */
    void Update(float deltaTime);

    std::shared_ptr<Utilities::JSON::JSONValue> Serialise() override;

    void Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json) override;

};

const std::unordered_map<std::string, Components::BaseComponent*> GameObject::GetComponents()
{
    return m_components;
}

GameObjectID GameObject::GetID()
{
    return ID;
}

bool GameObject::Enabled()
{
    return transform.GetParent() == nullptr ? m_enabled : m_enabled && transform.GetParent()->gameObject->Enabled();
}

void GameObject::SetEnabled(bool isEnabled)
{
    m_enabled = isEnabled;
}

bool GameObject::Static()
{
    return transform.GetParent() == nullptr ? m_static : m_enabled && transform.GetParent()->gameObject->Static();
}

void GameObject::SetStatic(bool isStatic)
{
    m_static = isStatic;
    m_scene->SetStatic(ID, isStatic);
}

template <class T>
inline T* GameObject::AddComponent()
{
    std::string type = typeid(T).name();
    if(m_components.contains(type)) return dynamic_cast<T*>(m_components[type]);
    T* component = Components::ComponentManager::AddComponent<T>();
    Components::BaseComponent* comp = dynamic_cast<Components::BaseComponent*>(component);
    comp->SetGameObject(this);
    m_components.insert({type, comp});
    return component;
}

template <class T>
inline T* GameObject::GetComponent()
{
    std::string type = typeid(T).name();
    if(m_components.contains(type)) return dynamic_cast<T*>(m_components[type]);
    return nullptr;
}

}