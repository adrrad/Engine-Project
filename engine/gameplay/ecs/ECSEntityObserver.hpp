#pragma once

#include "ECSDefinitions.hpp"

#include <set>

namespace Gameplay
{
    
class ECSEntityObserver
{
public:
    ECSEntityObserver(Signature signature);

    Signature GetSignature();

    void AddEntity(EntityID entity);

    void RemoveEntity(EntityID entity);

    const std::set<EntityID>& GetEntities();

protected:
    Signature m_signature;
    
    std::set<EntityID> m_entities;

};

} // namespace Gameplay
