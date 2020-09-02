#pragma once
#include "renderer/Transform.hpp"
#include "components/BaseComponent.hpp"
#include "components/ComponentPool.hpp"

#include <vector>
#include <string>
namespace Engine::Core
{

class GameObject
{
private:
    std::vector<Components::BaseComponent*> _components;
    
public:
    Rendering::Transform transform;
    std::string Name = "GameObject";
    template <class T>
    T* AddComponent();

    template <class T>
    T* GetComponent();


    void Update(float deltaTime);

};

template <class T>
inline T* GameObject::AddComponent()
{
    T* component = Components::ComponentManager::AddComponent<T>();
    BaseComponent* comp = dynamic_cast<BaseComponent*>(component);
    comp->SetGameObject(this);
    _components.push_back(component);
    return component;
}

template <class T>
inline T* GameObject::GetComponent()
{
    for(auto component : _components)
    {
        T* comp = dynamic_cast<T*>(component);
        if(comp != nullptr) return comp;
    }
    return nullptr;
}

}