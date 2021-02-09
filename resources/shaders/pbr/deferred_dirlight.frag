
float r;
float metalness;
vec3 F0;

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



void main()
{
    vec3 position = texture(gBuffer.position, Properties.UV).xyz;
    vec4 normMet = texture(gBuffer.normal, Properties.UV);
    vec4 colSpec = texture(gBuffer.albedoSpec, Properties.UV);
    float depth = texture(gBuffer.depth, Properties.UV).x;
    F0 = texture(gBuffer.reflectance, Properties.UV).xyz;
    vec3 N = normMet.rgb;
    vec3 colour = colSpec.rgb;
    metalness = normMet.a;
    r = colSpec.a;
    vec3 L = -directionalLight.Direction;
    vec3 V = normalize(camera.Position - position);
    vec3 H = normalize(L+V);
    vec3 col = BRDF_cook_torrance(colour, directionalLight.Colour.xyz, N, V, L, H)
    lColour = vec4(col + 0.03 * colour, 1.0f);
    // float brightness = dot(fragment_colour.rgb, vec3(0.2126, 0.7152, 0.0722));
    // if(brightness > 1.0)
    //     bright_colour = vec4(fragment_colour.rgb, 1.0);
    // else
    //     bright_colour = vec4(0.0, 0.0, 0.0, 1.0);
} 