
in vec3 o_pos;
in vec3 o_norm;

// struct PBRProperties
// {
//     vec4 u_colour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
//     float metalness = 0.7f;
//     float roughness = 0.1f;
//     vec3 F0 = vec3(0.96, 0.96, 0.97);
// };

uniform sampler2D albedo;
uniform sampler2D roughness;
uniform sampler2D metallic;

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
    float denom = nh2*(a2-1.0f)+1.0f;
    return a2/(PI*denom*denom);
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


vec3 cook_torrance(vec3 surfaceColour)
{
    float rough = texture(roughness, Properties.UV).x;
    vec3 F = mix(PBR.F0, surfaceColour, texture(metallic, Properties.UV).x);
    float D = normal_distribution(rough);
    F = fresnel(F);
    float a = rough;
    float k = (a*a+1.0f)/8.0f;
    float G = geometry_smith(k);
    
    return (D * F * G)/max((4 * max(dot(N,V), 0.0f) * max(dot(N,L), 0.0f)), 0.00001f);

}

vec3 BRDF_cook_torrance(vec3 surfaceColour)
{
    vec3 ks = fresnel(PBR.F0);
    vec3 kd = 1.0f - ks;
    kd *= 1.0f - texture(metallic, Properties.UV).x;
    vec3 spec = cook_torrance(surfaceColour);
    float nl = max(dot(N,L), 0.0f);
    return (kd * surfaceColour / PI + spec) * Renderer.Light.Colour.xyz * nl;
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
        N = CalculateNormalFromMap(Properties.UV);
        if(dot(N,Properties.V) < 0) N = -N;
        R = reflect(Properties.L, N);
    }
    vec3 colour = texture(albedo, Properties.UV).xyz;
    fragment_colour = vec4(BRDF_cook_torrance(colour), 1.0f);
    float reflectivity = Renderer.surface.EnvironmentReflectivity;
    
    if(Renderer.world.HasSkybox && reflectivity > 0.0)
    {
        vec3 norm = N.xyz;// CalculateNormalFromMap(Properties.UV + Renderer.Time*0.01).xyz;
        vec3 dir = o_pos - Renderer.camera.Position;
        vec3 reflectionVector = reflect(dir, norm);
        vec3 refractionVector = refract(dir, norm, 1.0f/1.55f);
        vec4 reflection = texture(Renderer.world.Skybox, reflectionVector);
        vec4 refraction = texture(Renderer.world.Skybox, refractionVector);
        float nv = max(dot(N, Properties.V),0.0);
        vec4 mixed = mix(reflection, refraction, nv);
        fragment_colour = mix(fragment_colour, mixed, reflectivity);
    }

    if(Renderer.surface.HasTexture)
    {
        // fragment_colour = texture(Renderer.surface.Texture, Properties.UV) + fragment_colour*0.00001f;
    }
    // fragment_colour = texture(albedo, Properties.UV) + fragment_colour*0.00001f;
    fragment_colour = fragment_colour / (fragment_colour + vec4(1.0));
    fragment_colour = pow(fragment_colour, vec4(1.0/2.2)); 
    // fragment_colour = vec4(Properties.UV, 0.0f, 0.0f) + fragment_colour*0.00001f;
    // fragment_colour = vec4(nv);
} 