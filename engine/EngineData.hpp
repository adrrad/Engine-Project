#pragma once

#include "EngineTypedefs.hpp"
#include "Array.hpp"

struct MeshIndices
{
    std::vector<u64> PositionIndices;
    std::vector<u64> NormalIndices;
    std::vector<u64> UVIndices;

    MeshIndices(const std::vector<u64>& p, std::vector<u64>& n, const std::vector<u64>& uv)
        : PositionIndices(p), NormalIndices(n), UVIndices(uv)
    {

    }

    MeshIndices() 
        : PositionIndices(0), NormalIndices(0), UVIndices(0)
    {

    }

    ~MeshIndices()
    {
        
    }

};

struct MeshData
{
    std::vector<float> Positions;
    std::vector<float> Normals;
    std::vector<float> UVs;
    std::vector<MeshIndices> Meshes;

    inline MeshData(std::vector<float>& ps, std::vector<float>& ns, std::vector<float>& uvs, std::vector<MeshIndices>& is) 
        : Positions(ps), Normals(ns), UVs(uvs), Meshes(is)
    {

    }
};


struct ImageData
{
    Array<unsigned char> Pixels;
    u32 Width;
    u32 Height;
    unsigned char NumChannels;

    inline ImageData(Array<unsigned char>& pixels, u32 width, u32 height, unsigned char numChannels)
        : Pixels(pixels)
    {
        Width = width;
        Height = height;
        NumChannels = numChannels;
    }
};

struct ShaderData
{
    const std::string& VertexSource;
    const std::string& FragmentSource;

    inline ShaderData(const std::string& vertex, const std::string& fragment) 
        : VertexSource(vertex), FragmentSource(fragment)
    {
        
    }
};
