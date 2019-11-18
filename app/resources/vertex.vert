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
out vec4 something;


void main()
{
    gl_Position = mesh.MVP * vec4(v_position, 1.0);
    something = vec4(v_normal,v_uv.x);
}