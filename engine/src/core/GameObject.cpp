#include "core/GameObject.hpp"

#include "utilities/serialisation/Serialisation.hpp"

#include <iostream>

namespace Engine::Utilities::Serialisation
{
    template<class C>
    void SerialiseProperty(C* object, const std::string& name, const std::vector<Components::BaseComponent*>& value)
    {
        std::string typeName = typeid(C).name();
        if(IsSerialised(typeName, name)) return;
        __int64 offset = (char*)&value - (char*)object;
        auto serializer = [offset, name](void* objPtr){
            std::vector<Components::BaseComponent*>* varloc = (std::vector<Components::BaseComponent*>*)((char*)objPtr+offset);
            std::vector<Components::BaseComponent*>& val = *varloc;
            std::vector<std::string> comps;
            for(auto comp : val)
            {
                comps.push_back(KeyValuePair(comp->Name, std::to_string(0)));
            }
            return KeyValuePair("components", JSONArray(comps));
        };
        AddSerializer(typeName,serializer);
        Serialiser::SerializedProps.insert(typeName+name);
    }
}


namespace Engine::Core
{

const std::vector<Components::BaseComponent*> GameObject::GetComponents()
{
    return _components;
}

void GameObject::SerialiseProperties()
{
    SERIALISE_PROPERTY(ID);
    SERIALISE_PROPERTY(Enabled);
    SERIALISE_PROPERTY(Name);
    SERIALISE_PROPERTY(_components);
    SERIALISE_PROPERTY(transform);
}

GameObject::GameObject() : ID(9999999)
{
    transform.gameObject = this;
}


GameObject::GameObject(GameObjectID id) : ID(id)
{
    transform.gameObject = this;
}

void GameObject::Update(float deltaTime)
{
    for(auto comp : _components)
    {
        comp->Update(deltaTime);
    }
}

} // namespace Engine::Rendering




