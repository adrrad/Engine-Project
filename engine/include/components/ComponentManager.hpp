#pragma once

#include "EngineTypedefs.hpp"
#include "core/EngineSubsystem.hpp"
#include "components/BaseComponent.hpp"
#include "utilities/json/JSON.hpp"
#include "utilities/StringUtilities.hpp"
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <string>

#define TypeToCharArray(T) #T

namespace Engine::Components
{


class IComponentPool
{
protected:
    std::string Name;
    uint64_t m_baseCapacity;
    uint64_t m_elementCount;
public:
    inline std::string GetName() { return Name; }
    virtual BaseComponent* AllocateNewComponent() = 0;
    virtual BaseComponent* GetComponent(ComponentID id) = 0;
    virtual void Start() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void DrawGUI() = 0;
}; 




template<typename T>
class ComponentPool : public IComponentPool
{
private:
    // struct ComponentsBlock
    // {

    // };


    std::vector<T*> m_components;
public:
    ComponentPool(Capacity baseCapacity = 100);
    std::vector<T*> GetComponents();
    BaseComponent* AllocateNewComponent() override;
    inline BaseComponent* GetComponent(ComponentID id) override;
    inline ElementCount GetComponentCount() { return m_components.size(); }
    void Update(float deltaTime) override;
    void Start() override;
    void DrawGUI() override;
};

template<typename T>
BaseComponent* ComponentPool<T>::GetComponent(ComponentID id)
{
    for(auto comp : m_components)
    {
        if(comp->GetID() == id) return comp;
    }
    return nullptr;
}

template<typename T>
ComponentPool<T>::ComponentPool(Capacity baseCapacity)
{
    Name = typeid(T).name();
    m_baseCapacity = baseCapacity;
}

template<typename T>
std::vector<T*> ComponentPool<T>::GetComponents()
{
    return m_components;
}

template<typename T>
BaseComponent* ComponentPool<T>::AllocateNewComponent()
{
    T* newComponent = new T();
    m_components.push_back(newComponent);
    return newComponent;
}

template<typename T>
void ComponentPool<T>::Update(float deltaTime)
{
    for(auto& component : m_components)
    {
        component->Update(deltaTime);
    }
}

template<typename T>
void ComponentPool<T>::Start()
{
    for(auto& component : m_components)
    {
        component->Start();
    }
}

template<typename T>
void ComponentPool<T>::DrawGUI()
{
    for(auto& component : m_components)
    {
        component->DrawGUI();
    }
}

class ComponentManager : public Engine::Core::EngineSubsystem
{
private:
    static ComponentManager* instance;

    static std::vector<IComponentPool*> _pools;
    static std::unordered_map<std::string, IComponentPool*> _mapping;
    ComponentManager() {};
public:
    static ComponentManager* GetInstance();

    inline static std::vector<IComponentPool*>& GetAllPools()
    {
        return _pools;
    }

    template<typename T>
    static void RegisterComponentPool(Capacity baseCapacity=100)
    {
        if(_mapping.contains(typeid(ComponentPool<T>).name())) return;
        ComponentPool<T>* pool = new ComponentPool<T>();
        _pools.push_back(pool);
        _mapping.insert({typeid(T).name(), pool});
        _mapping.insert({typeid(ComponentPool<T>).name(), pool});
    }

    template<typename T>
    static T* AddComponent()
    {
        static_assert(std::is_base_of<BaseComponent, T>::value, "Type not deriving from BaseComponent");
        //Static variables have individual instances per template function
        static ComponentPool<T>* pool = nullptr;
        if(pool == nullptr)
        {
            if(_mapping.contains(typeid(T).name()))
            {
                pool = dynamic_cast<ComponentPool<T>*>(_mapping[typeid(T).name()]);
            } 
            else
            {
                pool = new ComponentPool<T>();
                _pools.push_back(pool);
                _mapping.insert({typeid(ComponentPool<T>).name(), pool});
                _mapping.insert({typeid(T).name(), pool});
            }

        }
        BaseComponent* comp = pool->AllocateNewComponent();
        T* tcomp = dynamic_cast<T*>(comp);
        if(tcomp == nullptr) throw new std::exception("Component allocation went wrong!");
        tcomp->ID = ComponentID(pool->GetComponentCount() - 1);
        return tcomp;
    }

    template<typename T>
    static ComponentPool<T>* GetComponentPool()
    {
        static_assert(std::is_base_of<BaseComponent, T>::value, "Type not deriving from BaseComponent");
        static std::string typeName = typeid(ComponentPool<T>).name();
        if(_mapping.contains(typeName))
        {
            auto pool = _mapping[typeName];
            auto tpool = dynamic_cast<ComponentPool<T>*>(pool);
            if(tpool == nullptr) throw std::exception("Could not get the component pool!");
            return tpool;
        }
        return nullptr;
    }

    static IComponentPool* GetComponentPool(std::string name)
    {
        if(_mapping.contains(name))
        {
            return _mapping[name];
        }
        return nullptr;
    }

    static std::vector<IComponentPool*> GetComponentPools()
    {
        return _pools;
    }


    void Start();
    void Update(float deltaTime) override;
    static void DrawGUIAllComponents();
};


} // namespace Engine::Components