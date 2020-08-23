#version 430 core
#define MAX_LIGHTS 10
#define PI 3.1415926535897932384626433832795

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;
layout (location = 3) in vec3 v_tangent;
layout (location = 4) in vec3 v_bitangent;

out vec2 TexCoords;

void main()
{
    TexCoords = v_uv;
    gl_Position = vec4(v_position, 1.0);
}