#version 430 core
#define MAX_LIGHTS 10
#define PI 3.1415926535897932384626433832795
struct PointLight
{
vec4 Colour;
vec3 Position;
float Radius;
};
struct DirectionalLight
{
vec4 Colour;
vec3 Direction;
};
struct Camera
{
mat4 View;
mat4 Projection;
vec4 ClearColour;
vec3 Position;
};
struct StandardShadingProperties
{
vec4 N;
vec4 V;
vec4 L;
vec4 R;
vec4 H;
mat3 TBN;
vec4 ViewSpacePosition;
vec2 UV;
};
struct Textures
{
sampler2D normal;
sampler2D albedo;
sampler2D roughness;
sampler2D metallic;
sampler2D ambient;
};
uniform Textures textures;
layout(std140) uniform PBRProperties
{
vec3 F0;
bool hasNormal;
bool hasAO;
} PBR;
layout(std140) uniform InstanceUniforms
{
vec4 Shite;
mat4 Model;
mat4 ViewModel;
mat4 InvT;
mat4 MVP;
};
layout(std140) uniform GlobalUniforms
{
PointLight pointLights[10];
DirectionalLight directionalLight;
Camera camera;
int pointLightCount;
float time;
};
layout (location = 0) out vec4 fragment_colour;
layout (location = 1) out vec4 bright_colour;
in StandardShadingProperties Properties;
vec3 colour;
float a;
float r;
float m;

vec3 N;
vec3 V;
vec3 L;
vec3 R;
vec3 H;

vec4 CalculateNormalFromMap(vec2 uv)
{
    vec3 normal = texture(textures.normal, uv).xyz;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(Properties.TBN * normal);
    return vec4(normal,0.0f);
}

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
    vec3 L =  lpos - fragmentPosition;
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

    if(PBR.hasNormal)
    {
        N = CalculateNormalFromMap(Properties.UV).xyz;
        R = reflect(Properties.L.xyz, N);
    }
    colour = texture(textures.albedo, Properties.UV).xyz;
    r = texture(textures.roughness, Properties.UV).x;
    m = texture(textures.metallic, Properties.UV).x;
    if(PBR.hasAO) a = texture(textures.ambient, Properties.UV).x;
    else a = 0.0;

    vec3 plightShading = vec3(0.0f);
    vec3 red = vec3(0,0,0);
    for(int pli = 0; pli < pointLightCount; pli++)
    {
        plightShading += PointLightShading(colour, pointLights[pli], Properties.ViewSpacePosition.xyz);
    }

    vec3 col = BRDF_cook_torrance(colour, directionalLight.Colour.xyz, N, V, L, H) + plightShading;
    
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
    // fragment_colour = vec4(Model[0][0], Model[1][1], Model[2][2], 1.0f);
} 