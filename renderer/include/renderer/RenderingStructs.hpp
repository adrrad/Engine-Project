#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace Rendering
{
    
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 UV;

    Vertex() {};
    Vertex(glm::vec3 position) { Position = position; };
    Vertex(glm::vec3 position, glm::vec3 normal) { Position = position; Normal = normal;};
    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv) { Position = position; Normal = normal; UV = uv;};
};

struct LineSegment
{
    std::vector<glm::vec3> Vertices;
    glm::mat4 Transformation = glm::mat4(1.0f);
    glm::vec4 Colour = glm::vec4(1.0f);
    float Width = 1.0f;
};

struct Camera
{
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
    glm::vec4 BackgroundColour;
};

struct DirectionalLight
{
    glm::vec3 Direction;
    glm::vec4 Colour;
};

} // namespace Rendering
