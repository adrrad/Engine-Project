#pragma once

#include "EngineTypedefs.hpp"
#include "Array.hpp"

namespace Engine
{
    
struct MeshData
{
    Array<float> Positions;
    Array<float> Normals;
    Array<Index> Indices;
};

} // namespace Engine
