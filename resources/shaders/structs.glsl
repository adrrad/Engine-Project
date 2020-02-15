
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
    mat4 ViewModel;
    mat4 Model;
    mat4 InvT;
    mat4 MVP;
};

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

struct RendererUniforms
{
    int PointLightCount;
    DirectionalLight Light;
    PointLight PLights[MAX_LIGHTS];
    Camera camera;
    Mesh mesh;
    World world;
    Surface surface;
    float Time;
};