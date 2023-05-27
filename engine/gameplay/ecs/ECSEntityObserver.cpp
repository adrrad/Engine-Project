#include "ECSEntityObserver.hpp"

namespace Gameplay
{
    ECSEntityObserver::ECSEntityObserver(Signature signature)
    {
        m_signature = signature;
    }
    
    Signature ECSEntityObserver::GetSignature() 
    {
        return m_signature;
    }
    
    void ECSEntityObserver::AddEntity(EntityID entity) 
    {
        m_entities.insert(entity);
    }
    
    void ECSEntityObserver::RemoveEntity(EntityID entity) 
    {
        m_entities.erase(entity);
    }
    
    const std::set<EntityID>& ECSEntityObserver::GetEntities() 
    {
        return m_entities;
    }
}