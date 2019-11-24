#include "renderer/Mesh.hpp"


#include <glad/glad.h>


namespace Rendering
{

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, Shader* shader)
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
    
    glEnableVertexAttribArray(positionAttribLocation);
    glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Position));
    
    glEnableVertexAttribArray(normalAttribLocation);
    glVertexAttribPointer(normalAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(uvAttribLocation);
    glVertexAttribPointer(uvAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, UV));

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

Mesh* Mesh::GetParticlePlane(uint32_t length, uint32_t width, Shader* shader, float scale)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
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
    return new Mesh(vertices, indices, shader);
}

Mesh* Mesh::GetQuad(Shader* shader)
{
    std::vector<Vertex> vertices = {
        {{-0.5f, 0.5f, 0.0f}, {0,0,1}, { 0.0f, 0.0f }},
        {{-0.5f, -0.5f, 0.0f}, {0,0,1}, { 0.0f, 0.0f }},
        {{0.5f, 0.5f, 0.0f}, {0,0,1}, { 0.0f, 0.0f }},
        {{0.5f, -0.5f, 0.0f}, {0,0,1}, { 0.0f, 0.0f }},
    };
    std::vector<uint32_t> indices = { 0, 1, 2, 2, 1, 3};
    return new Mesh(vertices, indices, shader);
}

} // namespace Rendering
