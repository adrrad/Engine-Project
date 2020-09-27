#pragma once

#include "EngineTypedefs.hpp"
#include "Array.hpp"

namespace Engine
{

struct MeshIndices
{
    Array<Index> PositionIndices;
    Array<Index> NormalIndices;
    Array<Index> UVIndices;

    inline MeshIndices(const Array<Index>& p, const Array<Index>& n, const Array<Index>& uv)
        : PositionIndices(p), NormalIndices(n), UVIndices(uv)
    {

    }

    inline MeshIndices(const MeshIndices& other) 
        : MeshIndices(other.PositionIndices, other.NormalIndices, other.UVIndices)
    {

    }

};

struct MeshData
{
    Array<float> Positions;
    Array<float> Normals;
    Array<float> UVs;
    Array<MeshIndices> Indices;

    inline MeshData(Array<float>& ps, Array<float>& ns, Array<float>& uvs, Array<MeshIndices>& is) 
        : Positions(ps), Normals(ns), UVs(uvs), Indices(is)
    {

    }
};

} // namespace Engine
