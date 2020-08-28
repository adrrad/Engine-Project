
struct Camera
{
    mat4 View;
    mat4 Projection;
    vec4 ClearColour;
    vec3 Position;
};

struct Surface
{
    bool HasTexture;
    bool HasNormalMap;
    float EnvironmentReflectivity;
    // sampler2D Texture;
    // sampler2D NormalMap;
};

struct World
{
    bool HasSkybox;
    // samplerCube Skybox;
};

struct Mesh
{
    mat4 ViewModel;
    mat4 Model;
    mat4 InvT;
    mat4 MVP;
};



struct DirectionalLight
{
    vec4 Colour;
    vec3 Direction;
};

struct PointLight
{
    vec4 Colour;
    vec3 Position;
    float Radius;
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
    vec4 ViewSpacePosition;
};

struct PBRProperties
{
    vec4 Albedo;
    float Metallic;
    float Roughness;
    vec3 F0;
};

// struct RendererUniforms
// {
//     int PointLightCount;
//     DirectionalLight Light;
//     PointLight PLights[MAX_LIGHTS];
//     Camera camera;
//     Mesh mesh;
//     World world;
//     Surface surface;
//     float Time;
// };

layout(std140) uniform GlobalUniforms
{
    PointLight pointLights[MAX_LIGHTS];
    DirectionalLight directionalLight;
    Camera camera;
    bool hasSkybox;
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

uniform sampler2D Texture;
uniform sampler2D NormalMap;
uniform samplerCube Skybox;