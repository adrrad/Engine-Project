#pragma once
#include <glm/glm.hpp>

namespace Rendering
{
    
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 UV;
};

struct Camera
{
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
};

struct DirectionalLight
{
    glm::vec3 Direction;
    glm::vec4 Colour;
};

} // namespace Rendering
