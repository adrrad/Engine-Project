#version 430 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;

struct Camera
{
    vec3 Position;
    mat4 View;
    mat4 Projection;
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

uniform DirectionalLight r_u_dirLight;
uniform Camera r_u_camera;
uniform Mesh r_u_mesh;
uniform float r_u_time;


out vec4 something;
out vec4 colour;
out vec2 uv;
