#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace Engine::Rendering
{
    
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 UV;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

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
    LineSegment() { Vertices.reserve(10); }
};

struct LineInfo
{
    uint32_t VertexCount; 
    float Width; 
    glm::vec4 Colour;
};

struct Camera
{
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
    glm::vec4 BackgroundColour;
    glm::vec3 Position;
};

struct DirectionalLight
{
    glm::vec4 Colour;
    glm::vec3 Direction;
};

struct PointLight
{
    glm::vec4 Colour;
    glm::vec3 Position;
    float Radius;
};

struct InstanceUniforms
{
    glm::mat4x4 ViewModel;
    glm::mat4x4 Model;
    glm::mat4x4 InvT;
    glm::mat4x4 MVP;
    int HasTexture;
    int HasNormalMap;
    float EnvironmentReflectivity;
};

struct Ray
{
    glm::vec3 Origin;
    glm::vec3 Direction;
};


} // namespace Engine::Rendering
