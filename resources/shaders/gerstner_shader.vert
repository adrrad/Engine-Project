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

uniform vec4 u_colour;
uniform float u_amplitude;
uniform float u_waveLength;
uniform float u_waveSpeed;
uniform float u_directionAngle;


out vec4 something;
out vec4 colour;

vec3 normal;

float w;
float phase_const; //phase-constant
#define M_PI 3.1415926535897932384626433832795


vec3 WaveParticlePosition(vec3 pos)
{
    float angle = radians(u_directionAngle);
    vec2 dir = vec2(cos(angle), sin(angle));
    vec2 p = u_amplitude * sin(dir * pos.zx * w + r_u_time * phase_const);
    return vec3(p.x, p.y, 0.0f);
}

vec3 CalculateNormal()
{
    vec3 ofsByGerstnerWave = WaveParticlePosition(v_position);
    vec3 dx = vec3(0.01, 0, 0) + WaveParticlePosition(v_position + vec3(0.01, 0, 0));
    vec3 dz = vec3(0, 0, 0.01) + WaveParticlePosition(v_position + vec3(0, 0, 0.01));
    return normalize(cross(dz - ofsByGerstnerWave, dx - ofsByGerstnerWave));
}


void main()
{
    w = 2.0f/u_waveLength;
    phase_const = u_waveSpeed * w;
    gl_Position = r_u_mesh.MVP * (vec4(v_position, 1.0) + vec4(WaveParticlePosition(v_position), 0.0f));
    normal = -CalculateNormal();

    vec4 N = normalize(r_u_camera.View * r_u_mesh.Model * vec4(normal,0.0f));
    vec4 V = normalize(r_u_camera.View * r_u_mesh.Model * vec4(v_position, 1.0f));
    vec4 L = -normalize(r_u_camera.View * vec4(r_u_dirLight.Direction, 0.0f));
    if(dot(N,V) < 0) N = -N;
    vec4 R = normalize(reflect(-L,N));
    
    float diff = max(0.0f, dot(L,N)) * 0.3f;
    float spec = pow(max(0.0f, dot(V,R)), 20.0f);
    colour = u_colour * (vec4(diff) + vec4(spec));


    something = vec4(v_normal,v_uv.x);
}