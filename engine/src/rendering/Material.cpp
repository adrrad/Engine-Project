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
    m_shader = shader;
    m_vao = 0;
    m_instanceIndex = instanceIndex;
}

void Material::CreateVAO(BufferHandle vbo, BufferHandle ebo)
{
    if(m_vao != 0) glDeleteVertexArrays(1, &m_vao);
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    int positionAttribLocation = glGetAttribLocation(m_shader->GetProgramID(), "v_position");
    int normalAttribLocation = glGetAttribLocation(m_shader->GetProgramID(), "v_normal");
    int uvAttribLocation = glGetAttribLocation(m_shader->GetProgramID(), "v_uv");
    int tangentAttribLocation = glGetAttribLocation(m_shader->GetProgramID(), "v_tangent");
    int bitangentAttribLocation = glGetAttribLocation(m_shader->GetProgramID(), "v_bitangent");
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
    glDeleteVertexArrays(1, &m_vao);
    //TODO: Notify shader that this instance is gone
}

void Material::SetTexture(std::string name, Texture* texture)
{
    m_textures[name] = texture;
}

void Material::UseTextureAsset(std::string name, Assets::ImageAsset* textureAsset)
{
    m_textures[name] = Renderer::GetInstance()->GetTexture(textureAsset->ID);
}


} // namespace Engine::Rendering



