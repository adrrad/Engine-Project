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

uniform vec3 u_waveCenter;
uniform DirectionalLight dirLight;
uniform Camera camera;
uniform Mesh mesh;
uniform float time;


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
    vec3 x_t = (1.0f - (time - (pos.x)))*pos;
    return a*WaveFront(length(u_waveCenter - x_t), 5.0f);
}

float WaveHeight()
{
    normal = normalize(vec3(cos(abs(v_position.x - time)), -1.0f, 0.0f));
    vec3 delta = v_position + vec3(0.01f, 0.01, 0.0f);
    float deltaZ = Deviation(0.5f, v_position);
    delta = normalize(v_position - vec3(delta.x, delta.y, deltaZ));
    vec3 side = cross(delta, vec3(0,1,0));
    normal = cross(delta, side);
    return Deviation(0.5f, v_position);//(abs(v_position.x - time) * sin(abs(v_position.x - time)))/time;
}



void main()
{
    gl_Position = mesh.MVP * vec4(v_position, 1.0)+ vec4(0.0f, WaveHeight(), 0.0f, 0.0f);

    vec4 N = normalize(camera.View * mesh.Model * vec4(normal,0.0f));
    vec4 V = normalize(camera.View * mesh.Model * vec4(v_position, 1.0f));
    vec4 L = -normalize(camera.View * vec4(dirLight.Direction, 0.0f));
    //if(dot(N,V) < 0) N = -N;
    vec4 R = normalize(reflect(-L,N));
    
    float diff = max(0.0f, dot(L,N)) * 0.3f;
    float spec = pow(max(0.0f, dot(V,R)), 20.0f);
    colour = vec4(diff) + vec4(spec);


    something = vec4(v_normal,v_uv.x);
}