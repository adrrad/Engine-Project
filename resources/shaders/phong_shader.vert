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

uniform DirectionalLight dirLight;
uniform Camera camera;
uniform Mesh mesh;
uniform float time;


out vec4 something;
out vec4 colour;

vec3 normal;
float WaveHeight()
{
    normal = normalize(vec3(cos(abs(v_position.x - time)), -1.0f, 0.0f));

    return (abs(v_position.x - time) * sin(abs(v_position.x - time)))/time;
}

void main()
{
    gl_Position = mesh.MVP * vec4(v_position, 1.0);//+ vec4(0.0f, WaveHeight(), 0.0f, 0.0f);

    vec4 N = normalize(camera.View * mesh.Model * vec4(v_normal,0.0f));
    vec4 V = normalize(camera.View * mesh.Model * vec4(v_position, 1.0f));
    vec4 L = normalize(camera.View * vec4(dirLight.Direction, 0.0f));
    if(dot(N,V) < 0) N = -N;
    vec4 R = normalize(reflect(-L,N));
    
    float diff = max(0.0f, dot(L,N)) * 0.3f;
    float spec = pow(max(0.0f, dot(V,R)), 20.0f);
    colour = vec4(diff) + vec4(spec);


    something = vec4(v_normal,v_uv.x);
}