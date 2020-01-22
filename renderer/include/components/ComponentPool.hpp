#pragma once
#include "components/BaseComponent.hpp"

#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <string>

#define TypeToCharArray(T) #T

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
    std::vector<T> GetComponents();
    BaseComponent* AllocateNewComponent();
    void Update(float deltaTime);
    void DrawGUI();
};


template<typename T>
ComponentPool<T>::ComponentPool()
{

}

template<typename T>
std::vector<T> ComponentPool<T>::GetComponents()
{
    return _components;
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
    static std::unordered_map<const char*, IComponentPool*> _mapping;
public:
    template<typename T>
    static T* AddComponent()
    {
        static_assert(std::is_base_of<BaseComponent, T>::value, "Type not deriving from BaseComponent");
        //Static variables have individual instances per template function
        static ComponentPool<T>* pool = nullptr;
        if(pool == nullptr)
        {
            pool = new ComponentPool<T>();
            _pools.push_back(pool);
            _mapping.insert({typeid(T).name(), pool});
        }
        BaseComponent* comp = pool->AllocateNewComponent();
        T* tcomp = dynamic_cast<T*>(comp);
        if(tcomp == nullptr) throw new std::exception("Component allocation went wrong!");
        return tcomp;
    }
    template<typename T>
    static ComponentPool<T>* GetComponentPool()
    {
        static_assert(std::is_base_of<BaseComponent, T>::value, "Type not deriving from BaseComponent");
        const char* typeName = typeid(T).name();
        if(_mapping.contains(typeName))
        {
            auto pool = _mapping[typeName];
            auto tpool = dynamic_cast<ComponentPool<T>*>(pool);
            if(tpool == nullptr) throw std::exception("Could not get the component pool!");
            return tpool;
        } 
        // std::string err = std::string("No component pool for ") + std::string(typeName);
        // throw std::exception(err.c_str());
        return nullptr;
    }

    static void UpdateAllComponents(float deltaTime);
    static void DrawGUIAllComponents();
};


} // namespace Components


