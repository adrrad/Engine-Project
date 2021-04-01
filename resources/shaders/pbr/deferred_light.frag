
float r;
float metalness;
vec3 F0;

uniform int type;

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
    vec3 F = mix(F0, albedo, metalness);
    float D = normal_distribution(r, N, H);
    F = fresnel(F, V, H);
    float a = r;
    float k = ((a+1.0f)*(a+1.0f))/8.0f;
    float G = geometry_smith(k, N, V, L);
    
    return (D * F * G)/max((4 * max(dot(N,V), 0.0f) * max(dot(N,L), 0.0f)), 0.00001f);

}

vec3 BRDF_cook_torrance(vec3 albedo, vec3 lightColour, vec3 N, vec3 V, vec3 L, vec3 H)
{
    vec3 ks = fresnel(F0, V, H);
    vec3 kd = 1.0f - ks;
    kd *= 1.0f - metalness;
    vec3 spec = cook_torrance(albedo, N, V, L, H);
    float nl = max(dot(N,L), 0.0f);
    return (kd * albedo / PI + spec) * lightColour * nl;
}

vec3 PointLightShading(vec3 colour, PointLight p, vec3 fragmentPosition, vec3 N, vec3 V)
{
    vec3 lpos = p.Position;// vec3(camera.View * vec4(p.Position, 1.0f));
    vec3 L =  lpos - fragmentPosition;
    float dist = sqrt(dot(L,L));
    if(dist > p.Radius) return vec3(0.0f);
    L = L/dist;
    float intensity = p.Radius/(dist*dist);
    intensity = intensity*intensity;
    vec3 H = normalize(L+V);
    vec3 col = BRDF_cook_torrance(colour, p.Colour.xyz, N, V, L, H) * intensity;
    return col;
}

void main()
{
    vec3 position = texture(gBuffer.position, Geometry.UV).xyz;
    vec4 normMet = texture(gBuffer.normal, Geometry.UV);
    vec4 colSpec = texture(gBuffer.albedoSpec, Geometry.UV);
    float depth = texture(gBuffer.depth, Geometry.UV).x;
    F0 = texture(gBuffer.reflectance, Geometry.UV).xyz;
    metalness = normMet.a;
    r = colSpec.a;
    vec3 colour = colSpec.rgb;
    vec3 N = normMet.rgb;
    vec3 V = normalize(camera.Position - position);
    vec3 col = vec3(0);
    bool shadow = false;
    

    if(type == 0)
    {
        vec3 L = -directionalLight.Direction;
        vec3 H = normalize(L+V);
        col = BRDF_cook_torrance(colour, directionalLight.Colour.xyz, N, V, L, H);
        vec4 projCoords = directionalLight.ViewProjection * vec4(position, 1.0);
        projCoords = projCoords / projCoords.w;
        projCoords = projCoords * 0.5 + 0.5;
        if(projCoords.x >= 0 && projCoords.y >= 0 && projCoords.x <= 1 && projCoords.y <= 1)
        {
            float shadowdepth = texture(shadowmap.depth, projCoords.xy).r;
            // float bias = max(0.05 * (1.0 - dot(N, L)), 0.005);
            if(projCoords.z - 0.0005 > shadowdepth) col *= 0.0;
        }

    }
    else
    {
        col = PointLightShading(colour, pointLight, position, N, V);
    }

    lColour = vec4(col + 0.03 * colour, 1.0);
    // lColour = vec4(position, 1.0);
} 