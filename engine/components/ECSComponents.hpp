#pragma once

#include "gameplay/Transform.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/Material.hpp"
#include "geometry/AxisAlignedBox.hpp"

namespace Gameplay::ECS
{

const u32 MAX_TEXTURES_PER_MATERIAL = 6;

using TransformComponent = Transform;
using AxisAlignedBoxComponent = Geometry::AxisAlignedBox;

struct MeshComponent
{
    u32 VBO;
    u32 EBO;
    u32 VAO;
    u32 IndexCount;
};

struct MaterialComponent
{
    struct TextureBinding { u32 UniformLocation; u32 TextureID; };
    TextureBinding Textures[MAX_TEXTURES_PER_MATERIAL];
    Rendering::Material* MaterialInstance;
    u32 InstanceIndex;
    u32 ShaderID;
    u32 VAO;
    u32 NumTextures;
};



} // namespace Gameplay
