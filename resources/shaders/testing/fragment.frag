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

out vec4 fragment_colour;

layout(std140) uniform GlobalUniforms
{
    PointLight pointLights[MAX_LIGHTS];
    DirectionalLight directionalLight;
    Camera camera;
    int hasSkybox;
    // samplerCube skybox;
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
    fragment_colour = vec4(time); //pointLights[0].Colour;
}