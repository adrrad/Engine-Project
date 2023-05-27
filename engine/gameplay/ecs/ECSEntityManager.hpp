#pragma once

#include "ECSDefinitions.hpp"

#include <queue>
#include <array>

namespace Gameplay
{
    
class ECSEntityManager
{
public:
    ECSEntityManager();

    EntityID GetEntity();

    void ReleaseEntity(EntityID entity);

    void SetSignature(EntityID entity, Signature signature);

    Signature GetSignature(EntityID entity);

private:

    void Initialise();

    std::queue<EntityID> m_freeEntities;
    std::array<Signature, MAX_ENTITIES> m_signatures;

    u32 m_activeEntitiesCount;

};

} // namespace Gameplay


