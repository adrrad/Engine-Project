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
    bool HasNormalMap;
    sampler2D NormalMap;
    float EnvironmentReflectivity;
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
    mat3 TBN;
};

struct PBRProperties
{
    vec4 Albedo;
    float Metallic;
    float Roughness;
    vec3 F0;
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
uniform PBRProperties PBR;

in StandardShadingProperties Properties;

out vec4 fragment_colour;

vec4 CalculateNormalFromMap(vec2 offset = vec2(0.0))
{
    vec2 uvs = Properties.UV + offset;
    vec3 normal = texture(Renderer.surface.NormalMap, uvs).rbg;
    // normal.x = normal.x * 2.0 - 1.0;
    // normal.z = normal.z * 2.0 - 1.0;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(Properties.TBN * normal);
    return vec4(normal, 0.0f);
}