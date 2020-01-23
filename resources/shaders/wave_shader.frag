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

uniform Surface r_u_surface;
uniform World r_u_world;
uniform Camera r_u_camera;

in vec4 something;
in vec4 colour;
in vec2 uv;

// in vec3 reflection;

in vec3 o_pos;
in vec3 o_norm;
void main()
{
    fragment_colour = colour + something * 0.0001;
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
        fragment_colour = mix(refraction, reflection, 0.5);
    }
} 