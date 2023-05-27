#include "ECSSystemManager.hpp"

namespace Gameplay
{

    ECSSystemManager* ECSSystemManager::s_instance = nullptr;

    ECSSystemManager* ECSSystemManager::GetInstance() 
    {
        if(s_instance == nullptr)
        {
            s_instance = new ECSSystemManager();
        }
        return s_instance;
    }


    void ECSSystemManager::EntitySignatureChanged(EntityID entity, Signature entitySignature) 
    {
        for(auto const& pair : m_systems)
        {
            auto const& type = pair.first;
            auto const& system = pair.second;
            auto const& systemSignature = m_signatures[type];

            if((entitySignature & systemSignature) == systemSignature)
            {
                system->m_entities.insert(entity);
            }
            else
            {
                system->m_entities.erase(entity);
            }
        }
    }

    void ECSSystemManager::EntityDestroyed(EntityID entity)
    {
        for(auto const& pair : m_systems)
        {
            auto const& system = pair.second;
            system->m_entities.erase(entity);
        }
    }


}