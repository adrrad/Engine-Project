#version 430 core
#define MAX_LIGHTS 10
#define PI 3.1415926535897932384626433832795
uniform struct GBuffer
{
sampler2D position;
sampler2D normal;
sampler2D reflectance;
sampler2D albedoSpec;
sampler2D depth;
} gBuffer;
struct PointLight
{
vec4 Colour;
vec3 Position;
float Radius;
};
struct DirectionalLight
{
mat4 ViewProjection;
vec4 Colour;
vec3 Direction;
};
struct Camera
{
mat4 View;
mat4 Projection;
vec4 ClearColour;
vec3 Position;
};
struct StandardGeometry
{
vec4 N;
vec4 V;
vec4 L;
vec4 R;
vec4 H;
mat3 TBN;
vec4 ViewSpacePosition;
vec4 WorldSpacePosition;
vec2 UV;
};
layout(std140, binding=6) uniform CameraBuffer
{
Camera camera;
};
layout(std140, binding=3) uniform PLight
{
PointLight pointLight;
};
layout(std140, binding=4) uniform DLight
{
DirectionalLight directionalLight;
};
layout(std140, binding=0) uniform GlobalUniforms
{
vec2 viewportSize;
float time;
};
uniform struct Shadowmap
{
sampler2D depth;
} shadowmap;
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;
layout (location = 3) in vec3 v_tangent;
layout (location = 4) in vec3 v_bitangent;
out StandardGeometry Geometry;
void main()
{
    Geometry.UV = v_uv;
    gl_Position = vec4(v_position, 1.0);
};
