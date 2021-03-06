#version 430 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) in vec3 v_position;

uniform mat4 r_u_MVP;
uniform vec4 r_u_colour;

out vec4 colour;

void main()
{
    colour = r_u_colour;
    gl_Position = r_u_MVP * vec4(v_position, 1.0f);
}