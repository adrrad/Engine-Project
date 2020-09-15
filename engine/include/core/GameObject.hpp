#pragma once
#include "rendering/Transform.hpp"
#include "components/BaseComponent.hpp"
#include "components/ComponentManager.hpp"

#include <vector>
#include <string>

namespace Engine::GUI
{
    class SceneInspector;
}

namespace Engine::Core
{
class GameObject
{
friend class Engine::GUI::SceneInspector;
private:
    std::vector<Components::BaseComponent*> _components;
    
    const std::vector<Components::BaseComponent*> GetComponents();

public:
    bool Enabled = true; // TODO: Make a getter instead. That will allow to control whether children are enabled or not. Currently if parent is disabled the child remains unaffected.
    Rendering::Transform transform;
    std::string Name = "GameObject";

    GameObject();

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