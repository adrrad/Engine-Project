#version 430 core

#define MAX_LIGHTS 10

#define PI 3.1415926535897932384626433832795

struct DirectionalLight
{
    vec3 Direction;
    vec4 Colour;
};

struct PointLight
{
    vec4 Colour;
    vec3 Position;
    float Radius;
};

struct Camera
{
    vec3 Position;
    mat4 View;
    mat4 Projection;
};

struct Mesh
{
    mat4 ViewModel;
    mat4 Model;
    mat4 InvT;
    mat4 MVP;
};

struct Surface
{
    bool HasTexture;
    bool HasNormalMap;
    float EnvironmentReflectivity;
    // sampler2D Texture;
    // sampler2D NormalMap;
};

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;
layout (location = 3) in vec3 v_tangent;
layout (location = 4) in vec3 v_bitangent;

layout(std140) uniform GlobalUniforms
{
    PointLight pointLights[MAX_LIGHTS];
    DirectionalLight directionalLight;
    Camera camera;
    int hasSkybox;
    int pointLightCount;
    float time;
};

layout(std140) uniform InstanceUniforms
{
    mat4 ViewModel;
    mat4 Model;
    mat4 InvT;
    mat4 MVP;
    bool HasTexture;
    bool HasNormalMap;
    float EnvironmentReflectivity;
};

void main()
{
    gl_Position = MVP * vec4(v_position, 1.0f);
}