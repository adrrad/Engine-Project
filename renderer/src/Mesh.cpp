#include "renderer/Mesh.hpp"
#include "utilities/Utilities.hpp"


#include <glad/glad.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>


#include <iostream>

using namespace std;
using namespace glm;

namespace Rendering
{

Mesh::Mesh(vector<Vertex> vertices, vector<uint32_t> indices, Shader* shader)
{
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);
    glGenVertexArrays(1, &_vao);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    int positionAttribLocation = glGetAttribLocation(shader->GetID(), "v_position");
    int normalAttribLocation = glGetAttribLocation(shader->GetID(), "v_normal");
    int uvAttribLocation = glGetAttribLocation(shader->GetID(), "v_uv");
    int tangentAttribLocation = glGetAttribLocation(shader->GetID(), "v_tangent");
    int bitangentAttribLocation = glGetAttribLocation(shader->GetID(), "v_bitangent");
    
    glEnableVertexAttribArray(positionAttribLocation);
    glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Position));
    
    if(normalAttribLocation >= 0)
    {
        glEnableVertexAttribArray(normalAttribLocation);
        glVertexAttribPointer(normalAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Normal));
    }

    if(uvAttribLocation >= 0)
    {
        glEnableVertexAttribArray(uvAttribLocation);
        glVertexAttribPointer(uvAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, UV));
    }

    if(tangentAttribLocation >= 0)
    {
        glEnableVertexAttribArray(tangentAttribLocation);
        glVertexAttribPointer(tangentAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Tangent));
    }

    if(bitangentAttribLocation >= 0)
    {
        glEnableVertexAttribArray(bitangentAttribLocation);
        glVertexAttribPointer(bitangentAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Bitangent));
    }
    
    _vertexCount = uint32_t(vertices.size());
    _indexCount = uint32_t(indices.size());
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

uint32_t Mesh::GetVAO()
{
    return _vao;
}

uint32_t Mesh::GetVertexCount()
{
    return _vertexCount;
}
uint32_t Mesh::GetIndexCount()
{
    return _indexCount;
}

Mesh* Mesh::GetPlane(uint32_t length, uint32_t width, Shader* shader, float scale)
{
    vector<Vertex> vertices;
    vector<uint32_t> indices;
    uint32_t index = 0;
    for(uint32_t x = 0; x < width; x++)
    {
        for(uint32_t y = 0; y < length; y++)
        {
            float x_normalized = float(x)/float(width);
            float y_normalized = float(y)/float(length);
            float x_pos = (x_normalized - 0.5f) * scale;
            float z_pos = (y_normalized - 0.5f) * scale;
            Vertex v = {{x_pos, 0.0f, z_pos}, {0,1,0}, { x_normalized, y_normalized }};
            vertices.push_back(v);

            if(x < width - 1  && y < length - 1)
            {
                indices.push_back(index);
                indices.push_back(index + 1);
                indices.push_back(index + width);
                indices.push_back(index + width);
                indices.push_back(index + 1);
                indices.push_back(index + width + 1);
                index++;
            }
            else
            {
                index++;
            }
        }
    }
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
        T = normalize(T);

        B.x = f * (-dUV2.x * e1.x + dUV1.x * e2.x);
        B.y = f * (-dUV2.x * e1.y + dUV1.x * e2.y);
        B.z = f * (-dUV2.x * e1.z + dUV1.x * e2.z);
        B = normalize(B);

        v1.Tangent += T;
        v1.Bitangent += B;
        v2.Tangent += T;
        v2.Bitangent += B;
        v3.Tangent += T;
        v3.Bitangent += B;
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

    return new Mesh(vertices, indices, shader);
}

Mesh* Mesh::GetQuad(Shader* shader)
{
    vector<Vertex> vertices = {
        {{-0.5f, 0.5f, 0.0f}, {0,0,1}, { 0.0f, 0.0f }},
        {{-0.5f, -0.5f, 0.0f}, {0,0,1}, { 0.0f, 0.0f }},
        {{0.5f, 0.5f, 0.0f}, {0,0,1}, { 0.0f, 0.0f }},
        {{0.5f, -0.5f, 0.0f}, {0,0,1}, { 0.0f, 0.0f }},
    };
    vector<uint32_t> indices = { 0, 1, 2, 2, 1, 3};
    return new Mesh(vertices, indices, shader);
}

Mesh* Mesh::GetSkybox(Shader* shader)
{
    // TODO: FIX NORMALS AND UVs
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
    return new Mesh(vertices, indices, shader);
}

Mesh* Mesh::GetCube(Shader* shader)
{
    // TODO: FIX NORMALS AND UVs
    vector<Vertex> vertices = {
        //Front
        {{-1.0f, -1.0f, -1.0f}, {0.0f,0.0f,1.0f}, {0.0f, 1.0f}},
        {{-1.0f, 1.0f, -1.0f}, {0.0f,0.0f,1.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, -1.0f}, {0.0f,0.0f,1.0f}, {1.0f, 1.0f}},
        {{1.0f, 1.0f, -1.0f}, {0.0f,0.0f,1.0f}, {1.0f, 0.0f}},
        {{1.0f, -1.0f, -1.0f}, {0.0f,0.0f,1.0f}, {1.0f, 1.0f}},
        {{-1.0f, 1.0f, -1.0f}, {0.0f,0.0f,1.0f}, {0.0f, 0.0f}},
        //Right
        {{-1.0f, -1.0f, -1.0f}, {1.0f,0.0f,0.0f}, {1.0f, 1.0f}},
        {{-1.0f, -1.0f, 1.0f}, {1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        {{-1.0f, 1.0f, -1.0f}, {1.0f,0.0f,0.0f}, {1.0f, 0.0f}},
        {{-1.0f, 1.0f, 1.0f}, {1.0f,0.0f,0.0f}, {0.0f, 1.0f}},
        {{-1.0f, 1.0f, -1.0f}, {1.0f,0.0f,0.0f}, {1.0f, 1.0f}},
        {{-1.0f, -1.0f, 1.0f}, {1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        // Left
        {{1.0f, -1.0f, 1.0f}, {-1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, -1.0f}, {-1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 1.0f}, {-1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, -1.0f}, {-1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 1.0f}, {-1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, -1.0f}, {-1.0f,0.0f,0.0f}, {0.0f, 0.0f}},
        // Back
        {{-1.0f, 1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f, 0.0f}},
        {{-1.0f, -1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f, 0.0f}},
        {{-1.0f, -1.0f, 1.0f}, {0.0f,0.0f,-1.0f}, {0.0f, 0.0f}},
        // Top
        {{1.0f, 1.0f, -1.0f}, {0.0f,-1.0f,0.0f}, {0.0f, 0.0f}},
        {{-1.0f, 1.0f, -1.0f}, {0.0f,-1.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 1.0f}, {0.0f,-1.0f,0.0f}, {0.0f, 0.0f}},
        {{-1.0f, 1.0f, 1.0f}, {0.0f,-1.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 1.0f}, {0.0f,-1.0f,0.0f}, {0.0f, 0.0f}},
        {{-1.0f, 1.0f, -1.0f}, {0.0f,-1.0f,0.0f}, {0.0f, 0.0f}},
        // Bottom
        {{-1.0f, -1.0f, 1.0f}, {0.0f,1.0f,0.0f}, {0.0f, 0.0f}},
        {{-1.0f, -1.0f, -1.0f}, {0.0f,1.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, -1.0f}, {0.0f,1.0f,0.0f}, {0.0f, 0.0f}},
        {{-1.0f, -1.0f, 1.0f}, {0.0f,1.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, -1.0f}, {0.0f,1.0f,0.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, 1.0f}, {0.0f,1.0f,0.0f}, {0.0f, 0.0f}},
    };
    vector<uint32_t> indices;
    for(uint32_t index = 0; index < vertices.size(); index++)
    {
        indices.push_back(index);
    }
    return new Mesh(vertices, indices, shader);
}

Mesh* Mesh::GetSphere(Shader* shader)
{
    string spherepath = Utilities::GetAbsoluteResourcesPath("\\models\\sphere.obj");
    return FromFile(spherepath, shader);
}

Mesh* Mesh::FromFile(string path, Shader* shader)
{
    vector<Vertex> vertices;
    vector<uint32_t> indices;
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;
    string warn;
    string err;
    if(tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
    {
        
        for(uint32_t vertexIndex = 0; vertexIndex < attrib.vertices.size()/3; vertexIndex++)
        {
            Vertex v = {{0,0,0},{0,0,0},{0,0}};
            v.Position.x = attrib.vertices[vertexIndex*3];
            v.Position.y = attrib.vertices[vertexIndex*3+1];
            v.Position.z = attrib.vertices[vertexIndex*3+2];
            vertices.push_back(v);
        }

        tinyobj::shape_t& shape = shapes[0];
        for(tinyobj::index_t index : shape.mesh.indices)
        {
            int v_index = index.vertex_index;
            int n_index = index.normal_index;
            int uv_index = index.texcoord_index;
            Vertex& v = vertices[v_index];
            if(v.Normal.x == 0.0f && v.Normal.y == 0.0f && v.Normal.z == 0.0f)
            {
                v.Normal = {attrib.normals[n_index*3], attrib.normals[n_index*3+1], attrib.normals[n_index*3+2]};
                v.Normal = glm::normalize(v.Normal);
            }
            else
            {
                auto newNormal = glm::vec3(attrib.normals[n_index*3], attrib.normals[n_index*3+1], attrib.normals[n_index*3+2]);
                v.Normal += newNormal;
                v.Normal = glm::normalize(v.Normal);
            }
            
            
            v.UV = {attrib.texcoords[uv_index*2], attrib.texcoords[uv_index*2+1]};
            indices.push_back(v_index);
        }
    }
    else
    {
        cerr << "Failed to load " << path.c_str() << endl;
        return nullptr;
    }
    if (!warn.empty()) {
    cout << "WARN: " << warn << endl;
    }
    if (!err.empty()) {
        cerr << err << endl;
    }

    return new Mesh(vertices, indices, shader);

}

} // namespace Rendering
