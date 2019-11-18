#pragma once
#include "renderer/Transform.hpp"
#include "renderer/Mesh.hpp"
#include "components/BaseComponent.hpp"

#include <vector>

namespace Rendering
{

class SceneObject
{
private:
    std::vector<Components::BaseComponent*> _components;

public:
    Mesh *_mesh;
    Transform _transform;
    
    template <class T>
    T* AddComponent();

    template <class T>
    T* GetComponent();

};

template <class T>
inline T* SceneObject::AddComponent()
{
    T* component = new T(this);
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