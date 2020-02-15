
in vec3 o_pos;
in vec3 o_norm;

uniform vec4 u_colour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
uniform float metalness = 1.0f;
uniform float roughness = 0.1f;
uniform vec3 F0 = vec3(0.2f);

vec4 N;
vec4 V;
vec4 L;
vec4 R;
vec4 H;

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
    return (kd * u_colour.xyz / PI + spec) * Renderer.Light.Colour.xyz * nl;
}

void main()
{
    N = Properties.N;
    V = Properties.V;
    L = Properties.L;
    R = Properties.R;
    H = Properties.H;

    if(Renderer.surface.HasNormalMap)
    {
        N = Renderer.camera.View * -CalculateNormalFromMap(Properties.UV);
        R = reflect(-Properties.L, N);
    }

    fragment_colour =  vec4(BRDF_cook_torrance(), 1.0f);
    if(Renderer.surface.HasTexture)
    {
        fragment_colour *= texture(Renderer.surface.Texture, Properties.UV);
    }
    if(Renderer.world.HasSkybox)
    {
        vec3 ks = fresnel(F0);
        vec3 norm = -CalculateNormalFromMap(Properties.UV).xyz;
        vec3 reflectionVector = reflect(o_pos - Renderer.camera.Position, norm);
        vec3 refractionVector = refract(o_pos - Renderer.camera.Position, norm, 1.0f/1.55f);
        vec4 reflection = texture(Renderer.world.Skybox, reflectionVector);
        vec4 refraction = texture(Renderer.world.Skybox, refractionVector);
        // fragment_colour = mix(reflection, refraction, vec4(ks,1.0f));
    }
    fragment_colour = fragment_colour / (fragment_colour + vec4(1.0));
    fragment_colour = pow(fragment_colour, vec4(1.0/2.2)); 
} 