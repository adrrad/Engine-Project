#include "components/BaseComponent.hpp"
#include "core/GameObject.hpp"

namespace Components
{

void BaseComponent::SetGameObject(Rendering::GameObject* GameObject)
{
    this->gameObject = GameObject;
}

BaseComponent::BaseComponent(Rendering::GameObject* GameObject)
{
    this->gameObject = GameObject;
}

} // namespace Components
