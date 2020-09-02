#include "components/BaseComponent.hpp"
#include "core/GameObject.hpp"

namespace Components
{

void BaseComponent::SetGameObject(Rendering::GameObject* GameObject)
{
    this->GameObject = GameObject;
}

BaseComponent::BaseComponent(Rendering::GameObject* GameObject)
{
    this->GameObject = GameObject;
}

} // namespace Components
