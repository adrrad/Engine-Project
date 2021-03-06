#include "components/BaseComponent.hpp"
#include "core/GameObject.hpp"

namespace Engine::Components
{

void BaseComponent::SetGameObject(Engine::Core::GameObject* GameObject)
{
    this->gameObject = GameObject;
}

bool BaseComponent::Enabled()
{ 
    return this->enabled && gameObject->Enabled(); 
}


std::shared_ptr<Utilities::JSON::JSONValue> BaseComponent::Serialise()
{
    using namespace Utilities::JSON;

    return JSONValue::AsObject({
        { "name", JSONValue::AsString(Name)},
        { "id", JSONValue::AsString(ID.ToString())},
        { "enabled", JSONValue::AsBoolean(enabled) }
    });
}

void BaseComponent::Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json)
{
    Name = json->GetMember("name")->String;
    ID = json->GetMember("id")->String;
    enabled = json->GetMember("enabled")->Boolean;
}

} // namespace Engine::Components
