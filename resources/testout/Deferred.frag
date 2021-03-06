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
vec2 viewportSize;
float time;
};
layout(std140, binding=6) uniform CameraBuffer
{
Camera camera;
};
in StandardGeometry Geometry;
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec3 gReflectance;
layout (location = 3) out vec4 gAlbedoSpec;
layout (location = 4) out float gDepth;
vec4 CalculateNormalFromMap(vec2 uv)
{
    vec3 normal = texture(textures.normal, uv).xyz;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(Geometry.TBN * normal);
    return vec4(normal,0.0f);
}

void main()
{
   gPosition = Geometry.WorldSpacePosition.rgb;
   gNormal.rgb = CalculateNormalFromMap(Geometry.UV).rgb;
   gNormal.a = texture(textures.metallic, Geometry.UV).r;
   gReflectance = PBR.F0;
   gAlbedoSpec.rgb = texture(textures.albedo, Geometry.UV).rgb;
   gAlbedoSpec.a = texture(textures.roughness, Geometry.UV).r;
   gDepth = gl_FragCoord.z;
}
