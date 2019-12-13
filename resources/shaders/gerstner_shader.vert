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

//Uniforms set by the renderer
uniform DirectionalLight r_u_dirLight;
uniform Camera r_u_camera;
uniform Mesh r_u_mesh;
uniform float r_u_time;

//Shader specific uniforms
uniform vec4 u_colour = vec4(0,1,1,1);
uniform float u_amplitude = 1.5f;
uniform float u_waveLength = 5.0f;
uniform float u_waveSpeed = 0.5f;
uniform float u_directionAngle = 25.0f;

//Used to disable optimisation of unused variables
out vec4 something;

//Output colour
out vec4 colour;


float w;
float phase_const; //phase-constant
#define M_PI 3.1415926535897932384626433832795


vec3 WaveParticlePosition(vec3 pos)
{
    float angle = radians(u_directionAngle);
    vec2 dir = vec2(cos(angle), sin(angle));
    float y = u_amplitude * sin(dot(dir, pos.zx) * w + r_u_time * phase_const);
    return vec3(pos.x, y, pos.z);
}

vec3 CalculateNormal(vec3 position)
{
    vec3 dx = v_position + vec3(-0.01, 0, 0);
    vec3 dz = v_position + vec3(0, 0, -0.01);
    dx = WaveParticlePosition(dx);
    dz = WaveParticlePosition(dz);
    return normalize(cross(position - dz, position - dx));
}


void main()
{
    w = 2.0f/u_waveLength;
    phase_const = u_waveSpeed * w;
    vec4 wave_position = vec4(WaveParticlePosition(v_position), 1.0f);
    vec3 normal = CalculateNormal(wave_position.xyz);
    gl_Position = r_u_mesh.MVP * wave_position;
    // Calculate Phong inputs in view space
    vec4 N = normalize(r_u_camera.View * r_u_mesh.Model * vec4(normal, 0.0f));      //Surface normal
    vec4 V = -normalize(r_u_camera.View * r_u_mesh.Model * vec4(v_position, 1.0f)); //Surface to eye direction
    vec4 L = -normalize(r_u_camera.View * vec4(r_u_dirLight.Direction, 0.0f));      //Direction towards the light
    vec4 R = normalize(reflect(-L,N));                                              //Reflection vector
    
    float ambient = 0.1;
    float diff = max(dot(L,N), 0.0f);
    float spec = pow(max(dot(V,R), 0.0f), 32.0f);
    colour = 0.4*u_colour * (ambient + diff + spec);


    something = vec4(v_normal,v_uv.x)*spec;
}