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
    glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

    glEnableVertexAttribArray(normalAttribLocation);
    glVertexAttribPointer(normalAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

    glEnableVertexAttribArray(uvAttribLocation);
    glVertexAttribPointer(uvAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

    _vertexCount = uint32_t(vertices.size());
    _indexCount = uint32_t(indices.size());
    _shader = shader;
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Shader* Mesh::GetShader()
{
    return _shader;
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


} // namespace Rendering
