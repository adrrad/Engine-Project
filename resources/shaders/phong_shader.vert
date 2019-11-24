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

uniform DirectionalLight r_u_dirLight;
uniform Camera r_u_camera;
uniform Mesh r_u_mesh;
uniform float r_u_time;


out vec4 something;
out vec4 colour;

vec3 normal;
float WaveHeight()
{
    normal = normalize(vec3(cos(abs(v_position.x - r_u_time)), -1.0f, 0.0f));

    return (abs(v_position.x - r_u_time) * sin(abs(v_position.x - r_u_time)))/r_u_time;
}

void main()
{
    gl_Position = r_u_mesh.MVP * vec4(v_position, 1.0);//+ vec4(0.0f, WaveHeight(), 0.0f, 0.0f);

    vec4 N = normalize(r_u_camera.View * r_u_mesh.Model * vec4(v_normal,0.0f));
    vec4 V = normalize(r_u_camera.View * r_u_mesh.Model * vec4(v_position, 1.0f));
    vec4 L = -normalize(r_u_camera.View * vec4(r_u_dirLight.Direction, 0.0f));
    if(dot(N,V) < 0) N = -N;
    vec4 R = normalize(reflect(-L,N));
    
    float diff = max(0.0f, dot(L,N)) * 0.3f;
    float spec = pow(max(0.0f, dot(V,R)), 20.0f);
    colour = vec4(diff) + vec4(spec);


    something = vec4(v_normal,v_uv.x);
}