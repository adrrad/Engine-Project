#version 430 core

#define PI 3.1415926535897932384626433832795

struct Camera
{
    vec3 Position;
    mat4 View;
    mat4 Projection;
};

struct Surface
{
    bool HasTexture;
    sampler2D Texture;
};

struct World
{
    bool HasSkybox;
    samplerCube Skybox;
};

struct Mesh
{
    mat4 Model;
    mat4 InvT;
    mat4 MVP;
};

struct DirectionalLight
{
    vec3 Direction;
    vec4 Colour;
};

struct StandardShadingProperties
{
    vec4 N;
    vec4 V;
    vec4 L;
    vec4 R;
    vec4 H;
    vec2 UV;
};

struct RendererUniforms
{
    DirectionalLight Light;
    Camera camera;
    Mesh mesh;
    World world;
    Surface surface;
    float Time;
};

uniform RendererUniforms Renderer;

in StandardShadingProperties Properties;

out vec4 fragment_colour;

