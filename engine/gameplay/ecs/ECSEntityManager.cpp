#include "ECSEntityManager.hpp"

#include "../../Debug.hpp"

namespace Gameplay
{
    ECSEntityManager::ECSEntityManager() 
    : m_activeEntitiesCount(0)
    , m_freeEntities()
    , m_signatures()
    {
        Initialise();
    }
    
    EntityID ECSEntityManager::GetEntity() 
    {
        Assert(m_activeEntitiesCount < MAX_ENTITIES, "ECSEntityManager - Max entity count reached.");

        EntityID id = m_freeEntities.front();
        m_freeEntities.pop();
        m_activeEntitiesCount++;
        return id;
    }
    
    void ECSEntityManager::ReleaseEntity(EntityID entity) 
    {
        Assert(entity < MAX_ENTITIES, "ECSEntityManager - Entity ID out of range.");
        
        m_signatures[entity].reset();
        m_freeEntities.push(entity);
        m_activeEntitiesCount--;

    }
    
    void ECSEntityManager::SetSignature(EntityID entity, Signature signature) 
    {
        Assert(entity < MAX_ENTITIES, "ECSEntityManager - Entity ID out of range.");

        m_signatures[entity] = signature;

    }
    
    Signature ECSEntityManager::GetSignature(EntityID entity) 
    {
        Assert(entity < MAX_ENTITIES, "ECSEntityManager - Entity ID out of range.");

        return m_signatures[entity];
    }
    
    void ECSEntityManager::Initialise() 
    {
        for(EntityID entity = 0; entity < MAX_ENTITIES; entity++)
        {
            m_freeEntities.push(entity);
        }
    }
}