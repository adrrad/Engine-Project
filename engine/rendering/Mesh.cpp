#include "Mesh.hpp"

#include "Renderer.hpp"
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
using namespace Utilities;

namespace Rendering
{
typedef Geometry::AxisAlignedBox AxisAlignedBox;

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
    for(u64 i : indices)
    {
        min = Utilities::Min(min, vertices[i].Position);
        max = Utilities::Max(max, vertices[i].Position);
    }
    m_boundingVolume = New<AxisAlignedBox>(min, max);
}

void Mesh::CreateBuffers()
{
    UPDATE_CALLINFO();
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);
    UPDATE_CALLINFO();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size()*sizeof(uint32_t), m_indices.data(), GL_STATIC_DRAW);
    UPDATE_CALLINFO();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::CreateVAO()
{
    if(m_vao != 0) glDeleteVertexArrays(1, &m_vao);
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    // layout (location = 0) in vec3 v_position;
    // layout (location = 1) in vec3 v_normal;
    // layout (location = 2) in vec2 v_uv;
    // layout (location = 3) in vec3 v_tangent;
    // layout (location = 4) in vec3 v_bitangent; 
    int positionAttribLocation = 0; //glGetAttribLocation(m_shader->GetProgramID(), "v_position");
    int normalAttribLocation = 1; //glGetAttribLocation(m_shader->GetProgramID(), "v_normal");
    int uvAttribLocation = 2; //glGetAttribLocation(m_shader->GetProgramID(), "v_uv");
    int tangentAttribLocation = 3; //glGetAttribLocation(m_shader->GetProgramID(), "v_tangent");
    int bitangentAttribLocation = 4; //glGetAttribLocation(m_shader->GetProgramID(), "v_bitangent");
    UPDATE_CALLINFO();
    glEnableVertexAttribArray(positionAttribLocation);
    glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Position));
    UPDATE_CALLINFO();
    glEnableVertexAttribArray(normalAttribLocation);
    glVertexAttribPointer(normalAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Normal));
    UPDATE_CALLINFO();
    glEnableVertexAttribArray(uvAttribLocation);
    glVertexAttribPointer(uvAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, UV));
    UPDATE_CALLINFO();
    glEnableVertexAttribArray(tangentAttribLocation);
    glVertexAttribPointer(tangentAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Tangent));
    UPDATE_CALLINFO();
    glEnableVertexAttribArray(bitangentAttribLocation);
    glVertexAttribPointer(bitangentAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Bitangent));
    UPDATE_CALLINFO();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

Mesh::Mesh(vector<Vertex> vertices, vector<uint32_t> indices, AssetID resourceID) : Resource(resourceID)
{
    m_vertexCount = uint32_t(vertices.size());
    m_indexCount = uint32_t(indices.size());
    m_vertices = vertices;
    m_indices = indices;
    CreateBuffers();
    CreateVAO();
    CalculateBoundingBox(vertices, indices);
}

uint32_t Mesh::GetVertexCount()
{
    return m_vertexCount;
}

uint32_t Mesh::GetIndexCount()
{
    return m_indexCount;
}

std::vector<Vertex>& Mesh::GetVertices()
{
    return m_vertices;
}

Geometry::Volume* Mesh::GetBoundingVolume()
{
    return m_boundingVolume;
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
    
    return EngineSubsystem<Renderer>::GetSubsystemAllocator()->New<Mesh>(vertices, indices, AssetID());
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
        indices.push_back(index);
    }
    return EngineSubsystem<Renderer>::GetSubsystemAllocator()->New<Mesh>(vertices, indices, AssetID());
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
    if(verticesPerSegment == -1) return { {EngineSubsystem<Renderer>::GetSubsystemAllocator()->New<Mesh>(vertices, indices, AssetID()), {{0,0}, {w,h}}} };
    vector<pair<Mesh*, pair<ivec2, ivec2>>> meshes;
    for(auto& segmentIndices : Acceleration::MeshProcessor::SubdivideTerrain(vertices, indices, 0, verticesPerSegment))
    {
        ivec2 min = {99999, 99999}, max = {0, 0};
        for(u64 i : segmentIndices)
        {
            vec2 colRow = {i / w, i % h};
            min = Utilities::Min(min, colRow);
            max = Utilities::Max(max, colRow);
        }
        meshes.push_back({EngineSubsystem<Renderer>::GetSubsystemAllocator()->New<Mesh>(vertices, segmentIndices, AssetID()), {min, max}});
    }
    return meshes;
}


} // namespace Rendering
