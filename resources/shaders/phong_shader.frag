#version 430 core
out vec4 fragment_colour;


struct Surface
{
    bool HasTexture;
    sampler2D Texture;
};

uniform Surface r_u_surface;

in vec4 something;
in vec4 colour;
in vec2 uv;
void main()
{
    fragment_colour = colour + something * 0.0001;
    if(r_u_surface.HasTexture)
    {
        fragment_colour *= texture(r_u_surface.Texture, uv);
    }
} 