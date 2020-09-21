#pragma once
#include "core/Transform.hpp"
#include "components/BaseComponent.hpp"
#include "components/ComponentManager.hpp"
#include "utilities/serialisation/Serialisation.hpp"

#include <vector>
#include <string>

namespace Engine::Editor
{
    class SceneInspector;
    class EditorCore;
}

namespace Engine::Utilities::Serialisation
{
    template<class C>
    std::vector<Components::BaseComponent*> SerialiseProperty(Offset offset, std::string memberName, std::vector<Components::BaseComponent*>& member)
    {
        if(!Serialiser::IsSerialised<C>(memberName))
        {
            auto serializer = [offset, memberName](void* objPtr, int indent){
                std::vector<Components::BaseComponent*>* varloc = (std::vector<Components::BaseComponent*>*)((char*)objPtr+offset);
                std::vector<Components::BaseComponent*>& val = *varloc;
                std::vector<std::string> comps;
                for(auto comp : val)
                {
                    comps.push_back(KeyValuePairObject(comp->Name, std::to_string(comp->GetID()), 0));
                }
                return KeyValuePair("components", JSONArray(comps), indent);
            };
            AddSerializer<C>(memberName, serializer);
        }
        return member;
    }
}

namespace Engine::Core
{
//FORWARD DECLARATIONS
class EngineCore;
class Scene;

class GameObject : public Utilities::Serialisation::Serialisable
{
friend class Engine::Editor::SceneInspector;
friend class Engine::Core::EngineCore;
friend class Engine::Core::Scene;
friend class Engine::Editor::EditorCore;
private:
    const SERIALISABLE(GameObject, GameObjectID, ID);
    
    SERIALISABLE(GameObject, std::vector<Components::BaseComponent*>, _components);
    
    const std::vector<Components::BaseComponent*> GetComponents();


    /**
     * @brief The appropriate constructor for GameObjects used in the final game.
     * 
     * @param id The game object ID. Defined by the instantiating scene object.
     */
    GameObject(GameObjectID id);


public:
    /**
     * @brief Constructor for a GameObject used by the editor or other utilities.
     * 
     */
    GameObject();
    bool Enabled = true; // TODO: Make a getter instead. That will allow to control whether children are enabled or not. Currently if parent is disabled the child remains unaffected.
    SERIALISABLE(GameObject, Transform, transform);
    SERIALISABLE(GameObject, std::string, Name);

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

    inline std::string GetSerialised(int indent) override;

};

template <class T>
inline T* GameObject::AddComponent()
{
    T* component = Components::ComponentManager::AddComponent<T>();
    Components::BaseComponent* comp = dynamic_cast<Components::BaseComponent*>(component);
    comp->SetGameObject(this);
    _components.push_back(component);
    return component;
}

template <class T>
inline T* GameObject::GetComponent()
{
    for(auto component : _components)
    {
        T* comp = dynamic_cast<T*>(component);
        if(comp != nullptr) return comp;
    }
    return nullptr;
}

std::string GameObject::GetSerialised(int indent)
{
    return Utilities::Serialisation::SerializeObject(this, indent);
}

}