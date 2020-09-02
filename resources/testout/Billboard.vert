#version 430 core
#define MAX_LIGHTS 10
#define PI 3.1415926535897932384626433832795
struct PointLight
{
vec4 Colour;
vec3 Position;
float Radius;
};
struct DirectionalLight
{
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
struct StandardShadingProperties
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
struct Textures
{
sampler2D normal;
sampler2D albedo;
sampler2D roughness;
sampler2D metallic;
sampler2D ambient;
};
uniform Textures textures;
layout(std140, binding=2) uniform PBRProperties
{
vec3 F0;
bool hasNormal;
bool hasAO;
} PBR;
layout(std140, binding=1) uniform InstanceUniforms
{
mat4 Model;
mat4 ViewModel;
mat4 InvT;
mat4 MVP;
};
layout(std140, binding=0) uniform GlobalUniforms
{
PointLight pointLights[10];
DirectionalLight directionalLight;
Camera camera;
vec2 viewportSize;
int pointLightCount;
float time;
};
uniform struct Billboard
{
sampler2D texture;
} billboard;
layout(std140, binding=3) uniform Billboardprops
{
vec2 size;
} bbProps;
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;
layout (location = 3) in vec3 v_tangent;
layout (location = 4) in vec3 v_bitangent;
out StandardShadingProperties Properties;
void main()
{
    mat4 mv = ViewModel;
    mv[0][0] = 1;
    mv[0][1] = 0;
    mv[0][2] = 0;
    mv[1][0] = 0;
    mv[1][1] = 1;
    mv[1][2] = 0;
    mv[2][0] = 0;
    mv[2][1] = 0;
    mv[2][2] = 1;
    Properties.UV = v_uv;
    vec3 pos = v_position;
    pos.xy *=  bbProps.size;
    gl_Position = (camera.Projection * mv) * vec4(pos, 1.0);
    gl_Position /= gl_Position.w;
    gl_Position.xy += pos.xy;
};
