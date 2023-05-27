#pragma once

#include "ECSDefinitions.hpp"
#include "ECSComponentArray.hpp"

#include <unordered_map>
#include <memory>

namespace Gameplay
{
    
class ECSComponentManager
{
public:
    ECSComponentManager();
    ~ECSComponentManager();

    template<class T>
    void RegisterComponent();

    template<class T>
    ComponentType GetComponentType();

    template<class T>
    T& AddComponent(EntityID entity);

    template<class T>
    void RemoveComponent(EntityID entity);

    template<class T>
    T& GetComponent(EntityID entity);

    void EntityDestroyed(EntityID entity);

private:
    
    template<class T>
    ECSComponentArray<T>* GetComponentArray();

    std::unordered_map<const char*, ComponentType> m_componentTypes;
    std::unordered_map<const char*, IECSComponentArray*> m_componentArrays;
    ComponentType m_nextComponentType;

};

template<class T>
void ECSComponentManager::RegisterComponent() 
{
    const char* typeName = typeid(T).name();
    Assert(m_componentTypes.find(typeName) == m_componentTypes.end(),  "ECSComponentManager - Component already registered.");
    m_componentTypes.insert({typeName, m_nextComponentType});
    m_componentArrays.insert({typeName, new ECSComponentArray<T>()});
    m_nextComponentType++;
}

template<class T>
ComponentType ECSComponentManager::GetComponentType() 
{
    const char* typeName = typeid(T).name();
    Assert(m_componentTypes.find(typeName) != m_componentTypes.end(),  "ECSComponentManager - Component not registered.");
    return m_componentTypes[typeName];
}

template<class T>
T& ECSComponentManager::AddComponent(EntityID entity) 
{
    return GetComponentArray<T>()->AssignComponent(entity);
}

template<class T>
void ECSComponentManager::RemoveComponent(EntityID entity) 
{
    GetComponentArray<T>()->RemoveComponent(entity);
}

template<class T>
T& ECSComponentManager::GetComponent(EntityID entity) 
{
    return GetComponentArray<T>()->GetComponent(entity);
}

template<class T>
ECSComponentArray<T>* ECSComponentManager::GetComponentArray() 
{
    const char* typeName = typeid(T).name();
    Assert(m_componentTypes.find(typeName) != m_componentTypes.end(),  "ECSComponentManager - Component not registered.");
    return static_cast<ECSComponentArray<T>*>(m_componentArrays[typeName]);
}

} // namespace Gameplay
