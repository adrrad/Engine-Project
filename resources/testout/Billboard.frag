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
layout (location = 0) out vec4 fragment_colour;
layout (location = 1) out vec4 bright_colour;
in StandardShadingProperties Properties;
void main()
{
   fragment_colour = texture(billboard.texture, Properties.UV);
}
