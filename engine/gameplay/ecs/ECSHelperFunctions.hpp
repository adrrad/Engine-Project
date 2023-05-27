#pragma once
#include "ECSManager.hpp"
#include "ECSDefinitions.hpp"
namespace Gameplay
{


template<class T>
T& GetComponent(EntityID entity)
{
    static ECSManager* manager = ECSManager::GetInstance();
    return manager->GetComponent<T>(entity);
} 

} // namespace Gameplay
