#version 430 core
out vec4 fragment_colour;

struct Camera
{
    vec3 Position;
    mat4 View;
    mat4 Projection;
};

struct Surface
{
    bool HasTexture;
    sampler2D Texture;
    float Reflectivity;
};

struct World
{
    bool HasSkybox;
    samplerCube Skybox;
};

struct DirectionalLight
{
    vec3 Direction;
    vec4 Colour;
};


uniform DirectionalLight r_u_dirLight;
uniform Surface r_u_surface;
uniform World r_u_world;
uniform Camera r_u_camera;
uniform vec4 u_colour = vec4(1,1,1,1);

in vec4 something;
in vec4 colour;
in vec2 uv;

in vec4 N;
in vec4 V;
in vec4 L;
in vec4 R;
in vec4 H;
// in vec3 reflection;

in vec3 o_pos;
in vec3 o_norm;
#define PI 3.1415926535897932384626433832795

uniform float metalness = 0.1f;
uniform float roughness = 0.5f;
uniform vec3 F0 = vec3(0.04);


float normal_distribution(float a)
{
    float a2 = a*a;
    float nh = max(dot(N,H),0.0f);
    float nh2 = nh*nh;
    return a2/(PI*(nh2*(a2-1.0f)+1.0f));
}

float geometry_schlick(float nv, float k)
{
    return nv/(nv*(1.0f-k)+k);
}

float geometry_smith(float k)
{
    float nv = max(dot(N,V), 0.0f);
    float nl = max(dot(N,L), 0.0f);
    return geometry_schlick(nv,k) * geometry_schlick(nl,k);
}

vec3 fresnel(vec3 F)
{
    return F + (1.0f - F) * pow(1.0f - max(dot(H,V), 0.0f), 5.0f);
}


vec3 cook_torrance()
{
    vec3 F = mix(F0, u_colour.xyz, metalness);
    float D = normal_distribution(roughness);
    F = fresnel(F);
    float a = roughness;
    float k = (a*a+1.0f)/8.0f;
    float G = geometry_smith(k);
    
    return (D * F * G)/max((4 * max(dot(N,V), 0.0f) * max(dot(N,L), 0.0f)), 0.00001f);

}

vec3 BRDF_cook_torrance()
{
    vec3 ks = fresnel(F0);
    vec3 kd = 1.0f - ks;
    kd *= 1.0f - metalness;
    vec3 spec = cook_torrance();
    float nl = max(dot(N,L), 0.0f);
    return (kd * u_colour.xyz / PI + spec) * r_u_dirLight.Colour.xyz * nl;
}

void main()
{
    float ambient = 0.1;
    float diff = max(dot(L,N), 0.0f);
    float spec = pow(max(dot(V,R), 0.0f), 32.0f);
    fragment_colour =  vec4(BRDF_cook_torrance(), 1.0f) + something * 0.0001;
    if(r_u_surface.HasTexture)
    {
        fragment_colour *= texture(r_u_surface.Texture, uv);
    }
    if(r_u_world.HasSkybox)
    {
        vec3 reflectionVector = reflect(o_pos - r_u_camera.Position, o_norm);
        vec3 refractionVector = refract(o_pos - r_u_camera.Position, o_norm, 1.0f/1.55f);
        vec4 reflection = texture(r_u_world.Skybox, reflectionVector);
        vec4 refraction = texture(r_u_world.Skybox, refractionVector);
        fragment_colour *= mix(refraction, reflection, 0.5);
    }
} 