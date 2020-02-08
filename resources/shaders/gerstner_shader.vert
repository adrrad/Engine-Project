#define MAX_WAVE_COUNT 20

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

//Shader specific uniforms
uniform vec4 u_colour = vec4(0,1,1,1);
uniform float u_amplitude = 0.3f;
uniform float u_waveLength = 2.0f;
uniform float u_waveSpeed = 1.0f;
uniform float u_directionAngle = 25.0f;
uniform float u_steepness = 0.5f;
uniform int u_waveCount = 0;
uniform WaveSource u_waveSources[MAX_WAVE_COUNT];


//https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch01.html


out vec3 o_pos;
out vec3 o_norm;
out vec3 o_camPos;

vec3 WaveParticlePosition(vec3 pos)
{
    float x = pos.x;
    float y = 0.0f;
    float z = pos.z;
    float time = Renderer.Time;
    for(int waveIndex = 0; waveIndex < u_waveCount; waveIndex++)
    {
        int type = u_waveSources[waveIndex].type;                       //Wave type (Directional, Circular)
        float A = u_waveSources[waveIndex].A;                           //Amplitude
        float Q = u_waveSources[waveIndex].Q;                           //Steepness
        float angle = radians(u_waveSources[waveIndex].directionAngle); //Angle of directional waves
        float w = u_waveSources[waveIndex].w;                           //Frequency
        float pc = u_waveSources[waveIndex].pc;                         //Phase constant
        vec2 dir;
        if(type == 0)
        {
            dir = vec2(cos(angle), sin(angle));
        }
        else
        {
            dir = normalize(u_waveSources[waveIndex].center - v_position.xz);
        }
         
        x += (Q * A * dir.x * cos(w * dot(dir, pos.xz) + pc * time));
        z += (Q * A * dir.y * cos(w * dot(dir, pos.xz) + pc * time));
        y += A * sin(w*dot(dir, pos.xz) + pc * time);
    }
    
    return vec3(x, y, z);
}

vec3 CalculateNormal(vec3 position)
{
    vec3 dx = v_position + vec3(-0.001, 0, 0);
    vec3 dz = v_position + vec3(0, 0, -0.001);
    dx = WaveParticlePosition(dx);
    dz = WaveParticlePosition(dz);
    dz = normalize(position - dz);
    dx = normalize(position - dx);
    return normalize(cross(dz, dx));
}


void main()
{
    vec4 wave_position = vec4(v_position,1.0f);
    vec3 normal = v_normal;
    if(u_waveCount > 0)
    {
        wave_position = vec4(WaveParticlePosition(v_position), 1.0f);
        normal = CalculateNormal(wave_position.xyz);
    } 
    CalculateStandardProperties();
    CalculateTBNMatrix(normal);
    Properties.N = normalize(Renderer.camera.View * Renderer.mesh.Model * vec4(normal, 0.0f));     
    gl_Position = Renderer.mesh.MVP * wave_position;
    o_pos = vec3(Renderer.mesh.Model * vec4(v_position, 1.0f));
    o_norm = vec3(Renderer.mesh.InvT * vec4(normal, 0.0f));
}