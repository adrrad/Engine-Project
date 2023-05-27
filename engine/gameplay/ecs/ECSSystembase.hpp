#pragma once

#include "ECSDefinitions.hpp"
#include "ECSComponentManager.hpp"
#include <set>

namespace Gameplay
{
class ECSSystemManager;

class ECSSystembase
{
friend class ECSSystemManager;

public:
    virtual void Update(float deltaTime) {};

    virtual Signature GetSystemSignature() = 0;

protected:

    std::set<EntityID> m_entities;
};



    
} // namespace Gameplay
