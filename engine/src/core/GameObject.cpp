#include "core/GameObject.hpp"

#include "utilities/serialisation/Serialisation.hpp"

#include <iostream>


namespace Engine::Core
{

const std::vector<Components::BaseComponent*> GameObject::GetComponents()
{
    return m_components;
}

GameObject::GameObject() : ID(9999999)
{
    transform.gameObject = this;
    Name = "GameObject";
}


GameObject::GameObject(GameObjectID id) : ID(id)
{
    transform.gameObject = this;
    Name = "GameObject";
}

void GameObject::Update(float deltaTime)
{
    for(auto comp : m_components)
    {
        comp->Update(deltaTime);
    }
}

} // namespace Engine::Rendering




