#include "core/GameObject.hpp"

namespace Engine::Core
{

const std::vector<Components::BaseComponent*> GameObject::GetComponents()
{
    return _components;
}

GameObject::GameObject()
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


} // namespace Rendering




