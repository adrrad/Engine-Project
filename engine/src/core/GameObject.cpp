#include "core/GameObject.hpp"

#include "utilities/serialisation/Serialisation.hpp"

#include <iostream>


namespace Engine::Core
{

const std::vector<Components::BaseComponent*> GameObject::GetComponents()
{
    return m_components;
}

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
    for(auto comp : m_components)
    {
        comp->Update(deltaTime);
    }
}


} // namespace Engine::Rendering




