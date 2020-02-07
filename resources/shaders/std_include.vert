#version 430 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

#define PI 3.1415926535897932384626433832795

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;

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

out StandardShadingProperties Properties;


void CalculateStandardProperties()
{
    Properties.N = normalize(Renderer.camera.View * Renderer.mesh.Model * vec4(v_normal, 0.0f));      //Surface normal
    Properties.V = -normalize(Renderer.camera.View * Renderer.mesh.Model * vec4(v_position, 1.0f)); //Surface to eye direction
    Properties.L = -normalize(Renderer.camera.View * vec4(Renderer.Light.Direction, 0.0f));      //Direction towards the light
    // if(dot(Properties.N,Properties.V) < 0) Properties.N = -Properties.N;
    Properties.R = normalize(reflect(-Properties.L,Properties.N));
    Properties.H = normalize(Properties.L+Properties.V); 
    Properties.H = normalize(Properties.L+Properties.V);
    Properties.UV = v_uv;
}
