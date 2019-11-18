#version 330 core
#extension GL_ARB_explicit_attrib_location : require

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;

out vec4 something;


void main()
{
    gl_Position = vec4(v_position, 1.0);
    something = vec4(v_normal,v_uv.x);
}