#include "ECSComponentManager.hpp"

namespace Gameplay
{

    ECSComponentManager::ECSComponentManager()
    : m_componentTypes()
    , m_componentArrays()
    , m_nextComponentType(0)
    {

    }
    
    ECSComponentManager::~ECSComponentManager() 
    {
        for(auto& pair : m_componentArrays)
        {
            auto componentArray = pair.second;
            delete componentArray;
        }
    }

    void ECSComponentManager::EntityDestroyed(EntityID entity) 
    {
        for(auto const& pair : m_componentArrays)
        {
            auto const& componentArray = pair.second;
            componentArray->EntityDestroyed(entity);
        }
    }
}