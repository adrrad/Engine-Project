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

} // namespace Engine::Components
