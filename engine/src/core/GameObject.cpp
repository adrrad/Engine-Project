#include "core/GameObject.hpp"


#include <iostream>


namespace Engine::Core
{


GameObject::GameObject() 
{
    transform.gameObject = this;
    Name = "GameObject";
    m_enabled = true;
    m_static = false;
}

GameObject::GameObject(GameObjectID id, Scene* scene) : m_scene(scene)
{
    ID = id;
    transform.gameObject = this;
    Name = "GameObject";
    m_enabled = true;
    m_static = false;
}

void GameObject::Update(float deltaTime)
{

}

std::shared_ptr<Utilities::JSON::JSONValue> GameObject::Serialise()
{
    using namespace Utilities::JSON;

    auto serialiseComponents = [&]()
    {
        std::vector<std::shared_ptr<JSONValue>> serialisedComps;
        for(auto keyvalue : m_components)
        {
            auto comp = keyvalue.second;
            serialisedComps.push_back(comp->Serialise());
        }
        return JSONValue::AsArray(serialisedComps);
    };

    return JSONValue::AsObject({
        { "name", JSONValue::AsString(Name) },
        { "id", JSONValue::AsString(ID.ToString()) },
        { "static", JSONValue::AsBoolean(m_static) },
        { "enabled", JSONValue::AsBoolean(m_enabled) },
        { "transform", transform.Serialise() },
        { "components", serialiseComponents() },
    });
}

void GameObject::Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json)
{
    auto comps = json->GetMember("components")->Array;
    Name = json->GetMember("name")->String;
    ID = json->GetMember("id")->String;
    m_static = json->GetMember("static")->Boolean;
    m_enabled = json->GetMember("enabled")->Boolean;
    transform.Deserialise(json->GetMember("transform"));
    for(auto comp : comps)
    {
        auto deserialised = Components::ComponentManager::GetInstance()->DeserialiseComponent(this, comp);
    }
}

} // namespace Engine::Rendering




