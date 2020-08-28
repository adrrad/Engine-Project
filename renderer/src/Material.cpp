#pragma once
#include "renderer/Material.hpp"
#include "renderer/Shader.hpp"

#include <glad/glad.h>

#include <iostream>
#include <algorithm>

using namespace std;

namespace Rendering
{

void Material::UpdateTextures()
{
    uint32_t texID = 0;
    for(auto pair : _textures)
    {
        std::string name = pair.first;
        Texture* tex = pair.second;
        glActiveTexture(GL_TEXTURE3+texID);
        glBindTexture(tex->GetType(), tex->GetID());
        _shader->SetInt(name, 3+texID);
        texID++;
    }
}

Material::Material(Shader* shader)
{
    _shader = shader;
    _uniforms = _shader->GetActiveUniforms();
}

Material::~Material()
{

}

uint32_t Material::GetUniformCount()
{
    return _uniforms.size();
}

UniformData* Material::GetUniform(uint32_t index)
{
    return &_uniforms[index];
}


UniformData* Material::GetUniform(std::string name)
{
    auto it = find_if(_uniforms.begin(), _uniforms.end(), [&](UniformData& d){
        if(d.Name.compare(name) == 0) return true;
        return false;
    });
    if(it == _uniforms.end()) return nullptr;
    return &(*it); //TODO: Is there any proper way to get the value from an iterator?
}

void Material::SetTexture(std::string name, Texture* texture)
{
    _textures[name] = texture;
}

void Material::SetTexture(Texture* texture)
{
    _texture = texture;
}

void Material::SetNormalMap(Texture* normalMap)
{
    _normalMap = normalMap;
}

Texture* Material::GetNormalMap()
{
    return _normalMap;
}

Texture* Material::GetTexture()
{
    return _texture;
}


void Material::UpdateUniforms()
{
    _shader->Use();
    UpdateTextures();
    // for(UniformData& uniform : _uniforms)
    // {
    //     if(uniform.Name.find("r_u_") != string::npos) continue; //Do not update uniforms updated by the renderer.
    //     switch(uniform.Type)
    //     {
    //         case GL_BOOL:
    //         _shader->SetInt(uniform.Name.c_str(), uniform.i);
    //         case GL_INT:
    //         _shader->SetInt(uniform.Name.c_str(), uniform.i);
    //         break;
    //         case GL_FLOAT:   
    //         _shader->SetFloat(uniform.Name.c_str(), uniform.f);
    //         break;
    //         case GL_FLOAT_VEC2:                
    //         _shader->SetVec2(uniform.Name.c_str(), uniform.f2);
    //         break;
    //         case GL_FLOAT_VEC3:                
    //         _shader->SetVec3(uniform.Name.c_str(), uniform.f3);
    //         break;
    //         case GL_FLOAT_VEC4:
    //         _shader->SetVec4(uniform.Name.c_str(), uniform.f4);
    //         break;
    //         default:
    //         break;
    //     }
    // }
}

} // namespace Rendering



