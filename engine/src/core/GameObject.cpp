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


} // namespace Engine::Rendering




