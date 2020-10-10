#include "rendering/Mesh.hpp"
#include "rendering/Debugging.hpp"

#include "geometry/AxisAlignedBox.hpp"

#include "acceleration/MeshProcessor.hpp"

#include "utilities/Utilities.hpp"
#include "utilities/MathUtils.hpp"
#include <glad/glad.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Array.hpp"
#include "utilities/MathUtils.hpp"
#include "utilities/ImageProcessing.hpp"
#include <iostream>

using namespace std;
using namespace glm;
using namespace Engine::Utilities;

namespace Engine::Rendering
{
typedef Engine::Geometry::AxisAlignedBox AxisAlignedBox;

void Mesh::CalculateTangents(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    // Compute tangents and bitangents
    for(uint32_t index = 0; index < indices.size(); index += 3)
    {
        uint32_t i1 = indices[index];
        uint32_t i2 = indices[index+1];
        uint32_t i3 = indices[index+2];

        Vertex& v1 = vertices[i1];
        Vertex& v2 = vertices[i2];
        Vertex& v3 = vertices[i3];
        
        glm::vec3 e1 = v2.Position - v1.Position;
        glm::vec3 e2 = v3.Position - v1.Position;
        
        glm::vec2 dUV1 = v2.UV - v1.UV;
        glm::vec2 dUV2 = v3.UV - v1.UV;
        
        glm::vec3 T;
        glm::vec3 B;

        float f = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

        T.x = f * (dUV2.y * e1.x - dUV1.y * e2.x);
        T.y = f * (dUV2.y * e1.y - dUV1.y * e2.y);
        T.z = f * (dUV2.y * e1.z - dUV1.y * e2.z);
        // T = normalize(T);

        B.x = f * (-dUV2.x * e1.x + dUV1.x * e2.x);
        B.y = f * (-dUV2.x * e1.y + dUV1.x * e2.y);
        B.z = f * (-dUV2.x * e1.z + dUV1.x * e2.z);;

        v1.Tangent += T;
        v1.Bitangent += B;
    }
    for(uint32_t index = 0; index < indices.size(); index += 3)
    {
        uint32_t i1 = indices[index];
        uint32_t i2 = indices[index+1];
        uint32_t i3 = indices[index+2];
        
        Vertex& v1 = vertices[i1];
        Vertex& v2 = vertices[i1];
        Vertex& v3 = vertices[i2];
        v1.Tangent = normalize(v1.Tangent);
        v1.Bitangent = normalize(v1.Bitangent);
        v2.Tangent = normalize(v1.Tangent);
        v2.Bitangent = normalize(v1.Bitangent);
        v3.Tangent = normalize(v1.Tangent);
        v3.Bitangent = normalize(v1.Bitangent);
    }

}

void Mesh::CalculateBoundingBox(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    vec3 min = vertices[indices[0]].Position, max = vertices[indices[0]].Position;
    for(Index i : indices)
    {
        min = Utilities::Min(min, vertices[i].Position);
        max = Utilities::Max(max, vertices[i].Position);
    }
    _boundingVolume = new AxisAlignedBox(min, max);
}

void Mesh::CreateBuffers()
{
    UPDATE_CALLINFO();
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);
    UPDATE_CALLINFO();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size()*sizeof(uint32_t), m_indices.data(), GL_STATIC_DRAW);
    UPDATE_CALLINFO();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
}

Mesh::Mesh(vector<Vertex> vertices, vector<uint32_t> indices, AssetID resourceID) : Resource(resourceID)
{
    _vertexCount = uint32_t(vertices.size());
    _indexCount = uint32_t(indices.size());
    m_vertices = vertices;
    m_indices = indices;
    CreateBuffers();
    CalculateBoundingBox(vertices, indices);
}

uint32_t Mesh::GetVertexCount()
{
    return _vertexCount;
}

uint32_t Mesh::GetIndexCount()
{
    return _indexCount;
}

std::vector<Vertex>& Mesh::GetVertices()
{
    return m_vertices;
}

Engine::Geometry::Volume* Mesh::GetBoundingVolume()
{
    return _boundingVolume;
}

Mesh* Mesh::GetQuad()
{
    vector<Vertex> vertices = {
        {{-1.0f, 1.0f, 0.0f}, {0,0,1}, { 0.0f, 1.0f }},
        {{-1.0f, -1.0f, 0.0f}, {0,0,1}, { 0.0f, 0.0f }},
        {{1.0f, 1.0f, 0.0f}, {0,0,1}, { 1.0f, 1.0f }},
        {{1.0f, -1.0f, 0.0f}, {0,0,1}, { 1.0f, 0.0f }},
    };
    vector<uint32_t> indices = { 0, 1, 2, 2, 1, 3};
    return new Mesh(vertices, indices, AssetID());
}

Mesh* Mesh::GetSkybox()
{
    vector<Vertex> vertices = {
        //Front
        {{-1.0f, 1.0f, -1.0f}, {0.0f,0.0f,1.0f}, {0.0f, 0.0f}},
        {{-1.0f, -1.0f, -1.0f}, {0.0f,0.0f,1.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, -1.0f}, {0.0f,0.0f,1.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, -1.0f}, {0.0f,0.0f,1.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, -1.0f}, {0.0f,0.0f,1.0f}, {0.0f, 0.0f}},
        {{-1.0f, 1.0f, -1.0f}, {0.0f,0.0f,1.0f}, {0.0f, 0.0f}},
        //Right
        {{-1.0f, -1.0f, 1.0f}, {1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        {{-1.0f, -1.0f, -1.0f}, {1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        {{-1.0f, 1.0f, -1.0f}, {1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        {{-1.0f, 1.0f, -1.0f}, {1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        {{-1.0f, 1.0f, 1.0f}, {1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        {{-1.0f, -1.0f, 1.0f}, {1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        // Left
        {{1.0f, -1.0f, -1.0f}, {-1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, 1.0f}, {-1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 1.0f}, {-1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 1.0f}, {-1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, -1.0f}, {-1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, -1.0f}, {-1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        // Back
        {{-1.0f, -1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f, 0.0f}},
        {{-1.0f, 1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f, 0.0f}},
        {{-1.0f, -1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f, 0.0f}},
        // Top
        {{-1.0f, 1.0f, -1.0f}, {0.0f,1.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, -1.0f}, {0.0f,1.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 1.0f}, {0.0f,1.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 1.0f}, {0.0f,1.0f,0.0f}, {0.0f, 0.0f}},
        {{-1.0f, 1.0f, 1.0f}, {0.0f,1.0f,0.0f}, {0.0f, 0.0f}},
        {{-1.0f, 1.0f, -1.0f}, {0.0f,1.0f,0.0f}, {0.0f, 0.0f}},
        // Bottom
        {{-1.0f, -1.0f, -1.0f}, {0.0f,-1.0f,0.0f}, {0.0f, 0.0f}},
        {{-1.0f, -1.0f, 1.0f}, {0.0f,-1.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, -1.0f}, {0.0f,-1.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, -1.0f}, {0.0f,-1.0f,0.0f}, {0.0f, 0.0f}},
        {{-1.0f, -1.0f, 1.0f}, {0.0f,-1.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, 1.0f}, {0.0f,-1.0f,0.0f}, {0.0f, 0.0f}},
    };
    vector<uint32_t> indices;
    for(uint32_t index = 0; index < vertices.size(); index++)
    {
        // vertices[index].Normal = -vertices[index].Normal;
        indices.push_back(index);
    }
    return new Mesh(vertices, indices, AssetID());
}

std::vector<std::pair<Mesh*, pair<glm::ivec2, glm::ivec2>>> Mesh::FromHeightmap(ImageData* heightmap, float xyscale, float maxHeight, float uvscale, int verticesPerSegment)
{
    ImageData* hm = heightmap;
    uint32_t w = hm->Width; 
    uint32_t h = hm->Height;
    Array2D<Vertex> vertex = Array2D<Vertex>(h, w);
    vector<Vertex> vertices = std::vector<Vertex>(w*h);
    vector<uint32_t> indices;
    uint32_t index = 0;
    uint32_t vertIndex = 0;
    unsigned char* d = hm->Pixels.Data();

    #define DIM 9
    float weights[DIM][DIM];

    for(int i = 0; i < DIM; i++)
        for(int j = 0; j < DIM; j++)
            weights[i][j] = 1.0f;

    SamplingKernel<DIM,DIM> kernel(weights);

    for(uint32_t y = 0; y < h; y++)
    {
        for(uint32_t x = 0; x < w; x++)
        {
            float height = KernelSample(x, y, kernel, hm).x/255.0f;
            float x_normalized = float(x)/float(w);
            float y_normalized = float(y)/float(h);
            float x_pos = (x_normalized) * float(w) * xyscale;
            float z_pos = (y_normalized) * float(h) * xyscale;
            Vertex v = {{x_pos, height*maxHeight, z_pos}, {0,1,0}, { x_normalized * uvscale, y_normalized * uvscale }};
            vertices[vertIndex] = v;
            vertIndex++;
            if(x < w - 1  && y < h - 1)
            {
                indices.push_back(index + w);
                indices.push_back(index + 1);
                indices.push_back(index);
                indices.push_back(index + w + 1);
                indices.push_back(index + 1);
                indices.push_back(index + w);
                index++;
            }
            else
            {
                index++;
            }
        }
    }
    CalculateTangents(vertices, indices);
    if(verticesPerSegment == -1) return { {new Mesh(vertices, indices, AssetID()), {{0,0}, {w,h}}} };
    vector<pair<Mesh*, pair<ivec2, ivec2>>> meshes;
    for(auto& segmentIndices : Engine::Acceleration::MeshProcessor::SubdivideTerrain(vertices, indices, 0, verticesPerSegment))
    {
        ivec2 min = {99999, 99999}, max = {0, 0};
        for(Index i : segmentIndices)
        {
            vec2 colRow = {i / w, i % h};
            min = Utilities::Min(min, colRow);
            max = Utilities::Max(max, colRow);
        }
        meshes.push_back({new Mesh(vertices, segmentIndices, AssetID()), {min, max}});
    }
    return meshes;
}


} // namespace Engine::Rendering
