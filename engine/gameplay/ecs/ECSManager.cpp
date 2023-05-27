#include "ECSManager.hpp"

namespace Gameplay
{
    ECSManager* ECSManager::s_instance = nullptr;


    ECSManager* ECSManager::GetInstance()
    {
        if(s_instance == nullptr)
        {
            s_instance = new ECSManager();
        }
        return s_instance;
    }
    
    EntityID ECSManager::GetEntity() 
    {
        return m_entityManager->GetEntity();
    }
    
    void ECSManager::ReleaseEntity(EntityID entity) 
    {
        m_entityManager->ReleaseEntity(entity);
    }
    
    void ECSManager::Initialise() 
    {
        m_entityManager = new ECSEntityManager();
        m_componentManager = new ECSComponentManager();
        m_systemManager = ECSSystemManager::GetInstance();
    }

    ECSManager::ECSManager(/* args */) 
    {
        Initialise();
    }
}