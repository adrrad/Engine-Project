#version 430 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;

struct Camera
{
    mat4 View;
    mat4 Projection;
};

struct Mesh
{
    mat4 Model;
    mat4 MVP;
};

struct DirectionalLight
{
    vec3 Direction;
    vec4 Colour;
};

vec3 u_waveCenter = vec3(-1000.0f, -1000.0f, 0.0f);
uniform DirectionalLight dirLight;
uniform Camera camera;
uniform Mesh mesh;
uniform float time;


out vec4 something;
out vec4 colour;

vec3 normal;
#define M_PI 3.1415926535897932384626433832795


vec4 WaveParticlePosition(float beta)
{
    float t = mod(time, M_PI*2) - M_PI;
    float x = t - beta * sin(t);
    float z = 0.5f * (cos(t) + 1.0f);
    return vec4(x, 0.0f, z, 0.0f);
}



void main()
{
    gl_Position = mesh.MVP * (vec4(v_position, 1.0));

    vec4 N = normalize(camera.View * mesh.Model * vec4(v_normal,0.0f));
    vec4 V = normalize(camera.View * mesh.Model * vec4(v_position, 1.0f));
    vec4 L = -normalize(camera.View * vec4(dirLight.Direction, 0.0f));
    if(dot(N,V) < 0) N = -N;
    vec4 R = normalize(reflect(-L,N));
    
    float diff = max(0.0f, dot(L,N)) * 0.3f;
    float spec = pow(max(0.0f, dot(V,R)), 20.0f);
    colour = vec4(diff) + vec4(spec);


    something = vec4(v_normal,v_uv.x);
}