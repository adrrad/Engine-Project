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
vec4 WorldSpacePosition;
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
layout(std140, binding=2) uniform PBRProperties
{
vec3 F0;
bool hasNormal;
bool hasAO;
} PBR;
layout(std140, binding=1) uniform InstanceUniforms
{
mat4 Model;
mat4 ViewModel;
mat4 InvT;
mat4 MVP;
};
layout(std140, binding=0) uniform GlobalUniforms
{
Camera camera;
vec2 viewportSize;
int pointLightCount;
float time;
};
uniform struct GBuffer
{
sampler2D position;
sampler2D normal;
sampler2D reflectance;
sampler2D albedoSpec;
sampler2D depth;
} gBuffer;
layout(std140, binding=3) uniform PLight
{
PointLight pointLight;
};
layout(std140, binding=3) uniform DLight
{
DirectionalLight directionalLight;
};
in StandardShadingProperties Properties;
layout (location = 0) out vec4 lColour;

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
    vec3 position = texture(gBuffer.position, Properties.UV).xyz;
    vec4 normMet = texture(gBuffer.normal, Properties.UV);
    vec4 colSpec = texture(gBuffer.albedoSpec, Properties.UV);
    float depth = texture(gBuffer.depth, Properties.UV).x;
    F0 = texture(gBuffer.reflectance, Properties.UV).xyz;
    vec3 N = normMet.rgb;
    vec3 colour = colSpec.rgb;
    metalness = normMet.a;
    r = colSpec.a;
    
    vec3 V = normalize(camera.Position - position);
    vec3 plightShading = PointLightShading(colour, pointLight, position, N, V);
    lColour = vec4(plightShading + 0.03 * colour, 1.0f);
} 