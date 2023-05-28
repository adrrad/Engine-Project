#pragma once

#include "EngineTypedefs.hpp"
#include "EngineComponent.hpp"
#include "components/BaseComponent.hpp"
#include "utilities/json/JSON.hpp"
#include "utilities/StringUtilities.hpp"
#include "platform/GUID.hpp"
#include "Debug.hpp"
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <string>

#define TypeToCharArray(T) #T

namespace Gameplay
{


class IComponentPool
{
protected:
    std::string Name;
    uint64_t m_baseCapacity;
    uint64_t m_u64;
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
    std::vector<T*> m_components;
public:
    ComponentPool(u64 baseCapacity = 100);
    std::vector<T*>& GetComponents();
    BaseComponent* AllocateNewComponent() override;
    inline BaseComponent* GetComponent(ComponentID id) override;
    inline u64 GetComponentCount() { return m_components.size(); }
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
ComponentPool<T>::ComponentPool(u64 baseCapacity)
{
    Name = Utilities::Split(typeid(T).name(), "::").back();
    m_baseCapacity = baseCapacity;
}

template<typename T>
std::vector<T*>& ComponentPool<T>::GetComponents()
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

class ComponentManager : public EngineSubsystem<ComponentManager>
{
friend class EngineSubsystem<ComponentManager>;

private:
    static ComponentManager* instance;

    static std::vector<IComponentPool*> m_pools;
    static std::unordered_map<std::string, IComponentPool*> m_mapping;
    ComponentManager() {};
public:

    void Initialise() override;

    inline static std::vector<IComponentPool*>& GetAllPools()
    {
        return m_pools;
    }

    template<typename T>
    static void RegisterComponentPool(u64 baseCapacity=100)
    {
        std::string compname = Utilities::Split(typeid(T).name(), "::").back();
        if(m_mapping.contains(compname)) return;
        ComponentPool<T>* pool = new ComponentPool<T>();
        m_pools.push_back(pool);
        m_mapping.insert({compname, pool});
    }

    template<typename T>
    static T* AddComponent()
    {
        static_assert(std::is_base_of<BaseComponent, T>::value, "Type not deriving from BaseComponent");
        //Static variables have individual instances per template function
        static ComponentPool<T>* pool = nullptr;
        std::string compname = Utilities::Split(typeid(T).name(), "::").back();
        if(pool == nullptr)
        {
            if(m_mapping.contains(compname))
            {
                pool = dynamic_cast<ComponentPool<T>*>(m_mapping[compname]);
            } 
            else
            {
                pool = new ComponentPool<T>();
                m_pools.push_back(pool);
                m_mapping.insert({compname, pool});
            }

        }
        BaseComponent* comp = pool->AllocateNewComponent();
        comp->Name = Utilities::Split(typeid(T).name(), "::").back();
        T* tcomp = dynamic_cast<T*>(comp);
        if(tcomp == nullptr) AssertFail("Component allocation went wrong!");
        tcomp->ID = Platform::GenerateGUID();
        return tcomp;
    }

    template<typename T>
    static ComponentPool<T>* GetComponentPool()
    {
        static_assert(std::is_base_of<BaseComponent, T>::value, "Type not deriving from BaseComponent");
        static std::string compname = Utilities::Split(typeid(T).name(), "::").back();
        if(m_mapping.contains(compname))
        {
            auto pool = m_mapping[compname];
            auto tpool = dynamic_cast<ComponentPool<T>*>(pool);
            if(tpool == nullptr) AssertFail("Could not get the component pool!");
            return tpool;
        }
        return nullptr;
    }

    BaseComponent* DeserialiseComponent(Gameplay::GameObject* go, std::shared_ptr<Utilities::JSON::JSONValue> json);

    static IComponentPool* GetComponentPool(std::string name)
    {
        if(m_mapping.contains(name))
        {
            return m_mapping[name];
        }
        return nullptr;
    }

    static std::vector<IComponentPool*> GetComponentPools()
    {
        return m_pools;
    }


    void Start();
    void Update(float deltaTime) override;
    static void DrawGUIAllComponents();
};


} // namespace Gameplay