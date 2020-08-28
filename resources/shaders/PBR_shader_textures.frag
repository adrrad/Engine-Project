
in vec3 o_pos;
in vec3 o_norm;

uniform sampler2D albedo;
uniform sampler2D roughness;
uniform sampler2D metallic;
uniform sampler2D ambient;
bool hasAO = false;

vec3 colour;
float a;
float r;
float m;

vec3 N;
vec3 V;
vec3 L;
vec3 R;
vec3 H;


float normal_distribution(float a, vec3 N, vec3 H)
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

float geometry_smith(float k, vec3 N, vec3 V, vec3 L)
{
    float nv = max(dot(N,V), 0.0f);
    float nl = max(dot(N,L), 0.0f);
    return geometry_schlick(nv,k) * geometry_schlick(nl,k);
}

vec3 fresnel(vec3 F, vec3 V, vec3 H)
{
    return F + (1.0f - F) * pow(1.0f - max(dot(H,V), 0.0f), 5.0f);
}


vec3 cook_torrance(vec3 albedo, vec3 N, vec3 V, vec3 L, vec3 H)
{
    vec3 F = mix(PBR.F0, albedo, m);
    float D = normal_distribution(r, N, H);
    F = fresnel(F, V, H);
    float a = r;
    float k = ((a+1.0f)*(a+1.0f))/8.0f;
    float G = geometry_smith(k, N, V, L);
    
    return (D * F * G)/max((4 * max(dot(N,V), 0.0f) * max(dot(N,L), 0.0f)), 0.00001f);

}

vec3 BRDF_cook_torrance(vec3 albedo, vec3 lightColour, vec3 N, vec3 V, vec3 L, vec3 H)
{
    vec3 ks = fresnel(PBR.F0, V, H);
    vec3 kd = 1.0f - ks;
    kd *= 1.0f - m;
    vec3 spec = cook_torrance(albedo, N, V, L, H);
    float nl = max(dot(N,L), 0.0f);
    return (kd * albedo / PI + spec) * lightColour * nl;
}

vec3 PointLightShading(vec3 colour, PointLight p, vec3 fragmentPosition)
{
    vec3 lpos = vec3(camera.View * vec4(p.Position, 1.0f));
    vec3 L = lpos - fragmentPosition;
    float dist = sqrt(dot(L,L));
    if(dist > p.Radius) return vec3(0.0f);
    vec3 N = Properties.N.xyz;
    vec3 V = Properties.V.xyz;
    L = L/dist;
    float intensity = p.Radius/(dist*dist);
    intensity = intensity*intensity;
    vec3 H = normalize(L+V);
    vec3 col = BRDF_cook_torrance(colour, p.Colour.xyz, N, V, L, H) * intensity;
    return col;
}

void main()
{
    N = Properties.N.xyz;
    V = Properties.V.xyz;
    L = Properties.L.xyz;
    R = Properties.R.xyz;
    H = Properties.H.xyz;

    if(HasNormalMap)
    {
        N = CalculateNormalFromMap(Properties.UV).xyz;
        // if(dot(N,Properties.V.xyz) < 0) N = -N;
        R = reflect(Properties.L.xyz, N);
    }
    colour = texture(albedo, Properties.UV).xyz;
    r = texture(roughness, Properties.UV).x;
    m = texture(metallic, Properties.UV).x;
    if(hasAO) a = texture(ambient, Properties.UV).x;
    else a = 0.0;

    vec3 plightShading = vec3(0.0f);
    for(int pli = 0; pli < pointLightCount; pli++)
    {
        plightShading += PointLightShading(colour, pointLights[pli], Properties.ViewSpacePosition.xyz);
    }

    vec3 col = BRDF_cook_torrance(colour, directionalLight.Colour.xyz, N, V, L, H) + plightShading;
    float reflectivity = EnvironmentReflectivity;
    
    if(hasSkybox && reflectivity > 0.0)
    {
        vec3 norm = N.xyz;// CalculateNormalFromMap(Properties.UV + Renderer.Time*0.01).xyz;
        vec3 dir = o_pos - camera.Position;
        vec3 reflectionVector = reflect(dir, norm);
        vec3 refractionVector = refract(dir, norm, 1.0f/1.55f);
        vec4 reflection = texture(Skybox, reflectionVector);
        vec4 refraction = texture(Skybox, refractionVector);
        float nv = max(dot(N, Properties.V.xyz),0.0);
        vec4 mixed = mix(reflection, refraction, nv);
        col = mix(col, mixed.xyz, reflectivity);
    }
    col += vec3(0.03) * a;
    // HDR tonemapping
    // col = col / (col + vec3(1.0));
    // gamma correct
    // col = pow(col, vec3(1.0/2.2));
    fragment_colour = vec4(col, 1.0f);
    float brightness = dot(fragment_colour.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        bright_colour = vec4(fragment_colour.rgb, 1.0);
    else
        bright_colour = vec4(0.0, 0.0, 0.0, 1.0);
} 