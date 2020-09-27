#pragma once

#include "EngineTypedefs.hpp"
#include "utilities/Array.hpp"

namespace Engine
{
    
struct MeshData
{
    Utilities::Array<float> Positions;
    Utilities::Array<float> Normals;
    Utilities::Array<Index> Indices;
};

} // namespace Engine
