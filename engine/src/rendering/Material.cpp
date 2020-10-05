#pragma once
#include "rendering/Material.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Debugging.hpp"
#include "rendering/Renderer.hpp"

#include <glad/glad.h>

#include <iostream>
#include <algorithm>

using namespace std;

namespace Engine::Rendering
{


Material::Material(Shader* shader, Index instanceIndex)
{
    _shader = shader;
    _vao = 0;
    _instanceIndex = instanceIndex;
}

void Material::CreateVAO(BufferHandle vbo, BufferHandle ebo)
{
    if(_vao != 0) glDeleteVertexArrays(1, &_vao);
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    int positionAttribLocation = glGetAttribLocation(_shader->GetID(), "v_position");
    int normalAttribLocation = glGetAttribLocation(_shader->GetID(), "v_normal");
    int uvAttribLocation = glGetAttribLocation(_shader->GetID(), "v_uv");
    int tangentAttribLocation = glGetAttribLocation(_shader->GetID(), "v_tangent");
    int bitangentAttribLocation = glGetAttribLocation(_shader->GetID(), "v_bitangent");
    UPDATE_CALLINFO();
    glEnableVertexAttribArray(positionAttribLocation);
    glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Position));
    UPDATE_CALLINFO();
    if(normalAttribLocation >= 0)
    {
        glEnableVertexAttribArray(normalAttribLocation);
        glVertexAttribPointer(normalAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Normal));
    }
    UPDATE_CALLINFO();
    if(uvAttribLocation >= 0)
    {
        glEnableVertexAttribArray(uvAttribLocation);
        glVertexAttribPointer(uvAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, UV));
    }
    UPDATE_CALLINFO();
    if(tangentAttribLocation >= 0)
    {
        glEnableVertexAttribArray(tangentAttribLocation);
        glVertexAttribPointer(tangentAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Tangent));
    }
    UPDATE_CALLINFO();
    if(bitangentAttribLocation >= 0)
    {
        glEnableVertexAttribArray(bitangentAttribLocation);
        glVertexAttribPointer(bitangentAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Bitangent));
    }
    UPDATE_CALLINFO();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Material::~Material()
{
    glDeleteVertexArrays(1, &_vao);
    //TODO: Notify shader that this instance is gone
}

void Material::SetTexture(std::string name, Texture* texture)
{
    _textures[name] = texture;
}

void Material::UseTextureAsset(std::string name, Assets::ImageAsset* textureAsset)
{
    _textures[name] = Renderer::GetInstance()->GetTexture(textureAsset->ID);
}


} // namespace Engine::Rendering



