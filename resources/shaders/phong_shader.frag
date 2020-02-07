in vec4 something;
in vec4 colour;
in vec2 uv;

void main()
{
    float ambient = 0.1;
    float diff = max(dot(Properties.L,Properties.N), 0.0f);
    float spec = pow(max(dot(Properties.V,Properties.R), 0.0f), 32.0f);
    fragment_colour = vec4(diff) + vec4(spec);
    if(Renderer.surface.HasTexture)
    {
        fragment_colour *= texture(Renderer.surface.Texture, Properties.UV);
    }
} 