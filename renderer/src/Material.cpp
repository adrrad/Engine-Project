#pragma once
#include "renderer/Material.hpp"
#include <glad/glad.h>

#include <iostream>
#include <algorithm>

using namespace std;

namespace Rendering
{

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
        if(it == _uniforms.end()) throw std::exception("No uniform with the given name!");
        return &(*it); //TODO: Is there any proper way to get the value from an iterator?
    }

    void Material::UpdateUniforms()
    {
        for(UniformData& uniform : _uniforms)
        {
            if(uniform.Name.find("r_u_")) continue;
            switch(uniform.Type)
            {
                case GL_FLOAT:   
                _shader->SetFloat(uniform.Name.c_str(), uniform.f);
                break;
                case GL_FLOAT_VEC2:                
                _shader->SetVec2(uniform.Name.c_str(), uniform.f2);
                break;
                case GL_FLOAT_VEC3:                
                _shader->SetVec3(uniform.Name.c_str(), uniform.f3);
                break;
                case GL_FLOAT_VEC4:
                _shader->SetVec4(uniform.Name.c_str(), uniform.f4);
                break;
                default:
                break;
            }
        }
    }

} // namespace Rendering



