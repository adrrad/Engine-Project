#pragma once
#include "renderer/Material.hpp"
#include "renderer/Shader.hpp"

#include <glad/glad.h>

#include <iostream>
#include <algorithm>

using namespace std;

namespace Rendering
{


Material::Material(Shader* shader, Index instanceIndex)
{
    _shader = shader;
    _instanceIndex = instanceIndex;
}

Material::~Material()
{

}

void Material::SetTexture(std::string name, Texture* texture)
{
    _textures[name] = texture;
}


} // namespace Rendering



