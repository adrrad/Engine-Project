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
int pointLightCount;
float time;
};
uniform struct Skybox
{
samplerCube texture;
} skybox;
uniform struct GBuffer
{
sampler2D position;
sampler2D normal;
sampler2D reflectance;
sampler2D albedoSpec;
sampler2D depth;
} gBuffer;
uniform struct LBuffer
{
sampler2D colour;
} lBuffer;
layout (location = 0) out vec4 fragment_colour;
layout (location = 1) out vec4 bright_colour;
in StandardShadingProperties Properties;
in vec3 coordinates;
void main()
{
   float depth = 1.0f - texture(gBuffer.depth, gl_FragCoord.xy/vec2(1600, 1024)).x;
   if(depth >= 0.99) fragment_colour = texture(skybox.texture, coordinates);
   else fragment_colour = texture(lBuffer.colour, gl_FragCoord.xy/vec2(1600, 1024));
}
