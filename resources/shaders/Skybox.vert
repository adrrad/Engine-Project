#version 430 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;

out vec3 textureCoordinates;

struct Camera
{
    mat4 View;
    mat4 Projection;
};

uniform Camera r_u_camera;

void main()
{
    textureCoordinates = v_position;
    gl_Position = r_u_camera.Projection * vec4(mat3(r_u_camera.View) * v_position, 1.0);
}