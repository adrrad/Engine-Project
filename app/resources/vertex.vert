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

uniform Camera camera;
uniform Mesh mesh;
uniform float time;

out vec4 something;


float WaveHeight()
{
    return (abs(v_position.x - time) * sin(abs(v_position.x - time)))/time;
}

void main()
{
    gl_Position = mesh.MVP * vec4(v_position, 1.0) + vec4(0.0f, WaveHeight(), 0.0f, 0.0f);
    something = vec4(v_normal,v_uv.x);
}