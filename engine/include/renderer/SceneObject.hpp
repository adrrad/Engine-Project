#pragma once
#include "renderer/Transform.hpp"
#include "renderer/Mesh.hpp"
#include "components/BaseComponent.hpp"
#include "components/ComponentPool.hpp"

#include <vector>
#include <string>
namespace Rendering
{

class SceneObject
{
private:
    std::vector<Components::BaseComponent*> _components;
    
public:
    Transform transform;
    std::string Name = "SceneObject";
    template <class T>
    T* AddComponent();

    template <class T>
    T* GetComponent();


    void Update(float deltaTime);

};

template <class T>
inline T* SceneObject::AddComponent()
{
    T* component = Components::ComponentManager::AddComponent<T>();
    BaseComponent* comp = dynamic_cast<BaseComponent*>(component);
    comp->SetSceneObject(this);
    _components.push_back(component);
    return component;
}

template <class T>
inline T* SceneObject::GetComponent()
{
    for(auto component : _components)
    {
        T* comp = dynamic_cast<T*>(component);
        if(comp != nullptr) return comp;
    }
    return nullptr;
}

}