#version 430 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

#define PI 3.1415926535897932384626433832795

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;
layout (location = 3) in vec3 v_tangent;
layout (location = 4) in vec3 v_bitangent;

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

out StandardShadingProperties Properties;


void CalculateTBNMatrix(vec3 normal)
{
    vec3 N = normalize(vec3(Renderer.mesh.Model * vec4(normal, 0.0f)));
    vec3 T = normalize(vec3(Renderer.mesh.Model * vec4(v_tangent, 0.0f)));
    // vec3 B = normalize(vec3(Renderer.mesh.Model * vec4(v_bitangent, 0.0f)));
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(N, T);
    Properties.TBN = transpose(mat3(T,B,N));
}

void CalculateStandardProperties()
{
    Properties.N = normalize(Renderer.camera.View * Renderer.mesh.Model * vec4(v_normal, 0.0f));      //Surface normal
    Properties.V = -normalize(Renderer.camera.View * Renderer.mesh.Model * vec4(v_position, 0.0f)); //Surface to eye direction
    Properties.L = normalize(Renderer.camera.View * vec4(Renderer.Light.Direction, 0.0f));      //Direction towards the light
    if(dot(Properties.N,Properties.V) < 0) Properties.N = -Properties.N;
    Properties.R = normalize(reflect(-Properties.L,Properties.N));
    Properties.H = normalize(Properties.L+Properties.V); 
    Properties.UV = v_uv;
    CalculateTBNMatrix(v_normal);
}
