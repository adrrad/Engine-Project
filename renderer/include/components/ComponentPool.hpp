#pragma once
#include "components/BaseComponent.hpp"
#include<vector>

namespace Components
{

class IComponentPool
{
public:
    virtual BaseComponent* AllocateNewComponent() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void DrawGUI() = 0;
}; 

template<typename T>
class ComponentPool : public IComponentPool
{
private:
    std::vector<T> _components;
public:
    ComponentPool();
    BaseComponent* AllocateNewComponent();
    void Update(float deltaTime);
    void DrawGUI();
};


template<typename T>
ComponentPool<T>::ComponentPool()
{

}

template<typename T>
BaseComponent* ComponentPool<T>::AllocateNewComponent()
{
    T& newComponent = _components.emplace_back();
    return &newComponent;
}

template<typename T>
void ComponentPool<T>::Update(float deltaTime)
{
    for(auto& component : _components)
    {
        component.Update(deltaTime);
    }
}

template<typename T>
void ComponentPool<T>::DrawGUI()
{
    for(auto& component : _components)
    {
        component.DrawGUI();
    }
}

class ComponentManager
{
private:
    static std::vector<IComponentPool*> _pools;
public:
    template<typename T>
    static T* AddComponent();
    static void UpdateAllComponents(float deltaTime);
    static void DrawGUIAllComponents();
};

template<typename T>
T* ComponentManager::AddComponent()
{
    static_assert(std::is_base_of<BaseComponent, T>::value, "Type not deriving from BaseComponent");
    //Static variables have individual instances per template function
    static ComponentPool<T>* pool = nullptr;
    if(pool == nullptr)
    {
        pool = new ComponentPool<T>();
        _pools.push_back(pool);
    }
    BaseComponent* comp = pool->AllocateNewComponent();
    T* tcomp = dynamic_cast<T*>(comp);
    if(tcomp == nullptr) throw new std::exception("Component allocation went wrong!");
    return tcomp;
}

} // namespace Components


