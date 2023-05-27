#pragma once

#include "ECSDefinitions.hpp"
#include "../../Debug.hpp"

#include <array>
#include <unordered_map>

namespace Gameplay
{

class IECSComponentArray
{
public:
	virtual ~IECSComponentArray() = default;
	virtual void EntityDestroyed(EntityID entity) = 0;
};

template<class T>
class ECSComponentArray : public IECSComponentArray
{
public:
    T& AssignComponent(EntityID entity);

    void RemoveComponent(EntityID entity);

    T& GetComponent(EntityID entity);

    void EntityDestroyed(EntityID entity);

private:
    std::array<T, MAX_ENTITIES> m_componentArray;
    std::unordered_map<EntityID, u64> m_entityToIndexMap;
    std::unordered_map<u64, EntityID> m_indexToEntityMap;
    u64 m_size;
};

template<class T>
T& ECSComponentArray<T>::AssignComponent(EntityID entity) 
{
    Assert(m_entityToIndexMap.find(entity) == m_entityToIndexMap.end(), "ECSComponentArray - Entity has componenet assigned already.");
    u64 nextIndex = m_size;
    m_entityToIndexMap[entity] = nextIndex;
    m_indexToEntityMap[nextIndex] = entity;
    m_componentArray[nextIndex] = T();
    m_size++;
    return m_componentArray[nextIndex];

}

template<class T>
void ECSComponentArray<T>::RemoveComponent(EntityID entity) 
{
    Assert(m_entityToIndexMap.find(entity) != m_entityToIndexMap.end(), "ECSComponentArray - Entity does not have component assigned.");
    u64 entityIndex = m_entityToIndexMap[entity];
    u64 backIndex = m_size - 1;
    EntityID backEntity = m_indexToEntityMap[backIndex];

    // Replace the removed component with the back component
    m_componentArray[entityIndex] = m_componentArray[backIndex];

    // Update mappings
    m_entityToIndexMap[backEntity] = entityIndex;
    m_indexToEntityMap[entityIndex] = backEntity;

    m_entityToIndexMap.erase(entity);
    m_indexToEntityMap.erase(backIndex);
    m_size--;

}

template<class T>
T& ECSComponentArray<T>::GetComponent(EntityID entity) 
{
    Assert(m_entityToIndexMap.find(entity) != m_entityToIndexMap.end(), "ECSComponentArray - Entity does not have component assigned.");
    return m_componentArray[m_entityToIndexMap[entity]];
}

template<class T>
void ECSComponentArray<T>::EntityDestroyed(EntityID entity) 
{
    if(m_entityToIndexMap.find(entity) != m_entityToIndexMap.end())
    {
        RemoveComponent(entity);
    }
}

} // namespace Gameplay
