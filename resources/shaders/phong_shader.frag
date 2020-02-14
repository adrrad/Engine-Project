in vec4 something;
in vec4 colour;
in vec2 uv;

void main()
{
    vec4 N;
    vec4 R;
    if(Renderer.surface.HasNormalMap)
    {
        N = Renderer.camera.View * -CalculateNormalFromMap(Properties.UV);
        R = reflect(-Properties.L, N);
    }
    else
    {
        N = Properties.N;
        R = Properties.R;
    }
    
    float ambient = 0.1;
    float diff = max(dot(Properties.L,N), 0.0f);
    float spec = pow(max(dot(Properties.V,Properties.R), 0.0f), 100000.0f);
    fragment_colour = vec4(ambient) + vec4(diff) + vec4(spec);
    fragment_colour *= Renderer.Light.Colour;
    if(Renderer.surface.HasTexture)
    {
        // fragment_colour *= texture(Renderer.surface.Texture, Properties.UV);
    }
    fragment_colour = fragment_colour / (fragment_colour + vec4(1.0));
    fragment_colour = pow(fragment_colour, vec4(1.0/2.2)); 
} 