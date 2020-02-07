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
uniform DirectionalLight Renderer.Light;
uniform Camera Renderer.camera;
uniform Mesh Renderer.mesh;
uniform float Renderer.Time;

uniform vec4 u_colour;


out vec4 something;
out vec4 colour;

vec3 normal;
#define M_PI 3.1415926535897932384626433832795

float Rectangle(float x)
{
    if(abs(x) < 1.0f/2.0f) return 1.0f;
    else if (abs(x) == 1.0f/2.0f) return 1.0f/2.0f;
    else return 0.0f;
}

float WaveFront(float u, float l)
{
    return (1.0f/2.0f)*(cos((2*M_PI*u)/l)+1.0f) + Rectangle(u/l);
}

float Deviation(float a, vec3 pos)
{
    vec3 x_t = (1.0f - (cos(Renderer.Time) - (pos.x)))*pos;
    u_waveCenter = u_waveCenter + vec3(10.0,10.0,0.0)*Renderer.Time;
    return a*WaveFront(length(u_waveCenter - x_t), 20.0f);
}

float WaveHeight()
{
    //normal = normalize(vec3(cos(abs(v_position.x - Renderer.Time)), -1.0f, 0.0f));
    vec3 delta = v_position + vec3(0.1f, 0.1, 0.0f);
    float deltaZ = Deviation(0.15f, v_position);
    delta = normalize(vec3(delta.x, delta.y, deltaZ) - v_position);
    vec3 side = cross(delta, vec3(0,0,1));
    normal = cross(delta, side);
    return Deviation(0.5f, v_position);//(abs(v_position.x - Renderer.Time) * sin(abs(v_position.x - Renderer.Time)))/Renderer.Time;
}



void main()
{
    gl_Position = Renderer.mesh.MVP * vec4(v_position, 1.0)+ vec4(0.0f, WaveHeight(), 0.0f, 0.0f);

    vec4 N = normalize(Renderer.camera.View * Renderer.mesh.Model * vec4(normal,0.0f));
    vec4 V = normalize(Renderer.camera.View * Renderer.mesh.Model * vec4(v_position, 1.0f));
    vec4 L = -normalize(Renderer.camera.View * vec4(Renderer.Light.Direction, 0.0f));
    if(dot(N,V) < 0) N = -N;
    vec4 R = normalize(reflect(-L,N));
    
    float diff = max(0.0f, dot(L,N)) * 0.3f;
    float spec = pow(max(0.0f, dot(V,R)), 20.0f);
    colour = u_colour * (vec4(diff) + vec4(spec));


    something = vec4(v_normal,v_uv.x);
}