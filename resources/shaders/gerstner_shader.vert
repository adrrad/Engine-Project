#version 430 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable
#define MAX_WAVE_COUNT 3
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

struct WaveSource
{
    int type;
    float Q;
    float A;
    float L;
    float w;
    float S;
    float pc;
    float directionAngle;
    float lifespan;
    float range;
    vec2 center;
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
uniform float u_steepness = 0.0f;
uniform int u_waveCount = 0;
uniform WaveSource u_waveSources[MAX_WAVE_COUNT];


//Used to disable optimisation of unused variables
out vec4 something;
//Output colour
out vec4 colour;

//https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch01.html

float w;
float phase_const; //phase-constant
float Qi;





vec3 WaveParticlePosition(vec3 pos)
{
    float angle = radians(u_directionAngle);
    vec2 dir = vec2(cos(angle), sin(angle));
    float y = u_amplitude * sin(dot(dir, pos.zx) * w + r_u_time * phase_const);
    return vec3(pos.x, y, pos.z);
}

vec3 WaveParticlePosition_v2(vec3 pos)
{
    float angle = radians(u_directionAngle);
    vec2 dir = vec2(cos(angle), sin(angle));
    float x = pos.x + (Qi * u_amplitude * dir.x * cos(w * dot(dir, pos.zx) + phase_const * r_u_time));
    float z = pos.z + (Qi * u_amplitude * dir.y * cos(w * dot(dir, pos.zx) + phase_const * r_u_time));
    float y = u_amplitude * sin(dot(w*dir, pos.zx) + phase_const * r_u_time);
    return vec3(x, y, z);
}

vec3 WaveParticlePosition_v3(vec3 pos)
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    for(int waveIndex = 0; waveIndex < u_waveCount; waveIndex++)
    {
        int type = u_waveSources[waveIndex].type;
        float A = u_waveSources[waveIndex].A;
        float Q = u_waveSources[waveIndex].Q;
        float angle = u_waveSources[waveIndex].directionAngle;
        float w = u_waveSources[waveIndex].w;
        float pc = u_waveSources[waveIndex].pc;
        vec2 dir;
        if(type == 0)
        {
            dir = vec2(cos(angle), sin(angle));
        }
        else
        {
            dir = normalize(u_waveSources[waveIndex].center - v_position.xz);
        }
         
        x += pos.x + (Q * A * dir.x * cos(w * dot(dir, pos.zx) + pc * r_u_time));
        z += pos.z + (Q * A * dir.y * cos(w * dot(dir, pos.zx) + pc * r_u_time));
        y += A * sin(dot(w*dir, pos.zx) + pc * r_u_time);
    }
    
    return vec3(x, y, z);
}

vec3 CalculateNormal(vec3 position)
{
    vec3 dx = v_position + vec3(-0.01, 0, 0);
    vec3 dz = v_position + vec3(0, 0, -0.01);
    dx = WaveParticlePosition_v3(dx);
    dz = WaveParticlePosition_v3(dz);
    return normalize(cross(position - dz, position - dx));
}


void main()
{
    w = 2.0f/u_waveLength;
    phase_const = u_waveSpeed * w;
    Qi = u_steepness/(w*u_amplitude);

    vec4 wave_position = vec4(WaveParticlePosition_v3(v_position), 1.0f);
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