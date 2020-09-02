#include "components/BaseComponent.hpp"
#include "core/GameObject.hpp"

namespace Components
{

void BaseComponent::SetGameObject(Engine::Core::GameObject* GameObject)
{
    this->gameObject = GameObject;
}

BaseComponent::BaseComponent(Engine::Core::GameObject* GameObject)
{
    this->gameObject = GameObject;
}

} // namespace Components
