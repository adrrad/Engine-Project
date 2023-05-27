#pragma once


#include "../../EngineTypedefs.hpp"

#include <bitset>

namespace Gameplay
{

using EntityID = u32;
using ComponentType = u32;

const EntityID MAX_ENTITIES = 5000;
const ComponentType MAX_COMPONENTS = 100;

using Signature = std::bitset<MAX_COMPONENTS>;
    
} // namespace Gameplay






