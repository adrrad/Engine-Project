#pragma once

#include "Mesh.hpp"
#include "Material.hpp"
#include "Texture.hpp"

namespace Rendering
{

struct Skybox
{
    Mesh* SkyboxMesh = nullptr;
    Material* SkyboxMaterial = nullptr;
    Texture* SkyboxTexture = nullptr;
};

} // namespace Rendering



