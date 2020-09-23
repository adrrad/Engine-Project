#include "components/BaseComponent.hpp"
#include "core/GameObject.hpp"

namespace Engine::Components
{

void BaseComponent::SetGameObject(Engine::Core::GameObject* GameObject)
{
    this->gameObject = GameObject;
}

} // namespace Engine::Components
