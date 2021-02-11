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
mat4 ViewProjection;
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
struct StandardGeometry
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
vec2 viewportSize;
float time;
};
layout(std140, binding=6) uniform CameraBuffer
{
Camera camera;
};
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;
layout (location = 3) in vec3 v_tangent;
layout (location = 4) in vec3 v_bitangent;
out StandardGeometry Geometry;
void CalculateTBNMatrix(vec3 normal)
{
    vec3 N = normalize(vec3(Model * vec4(normal, 0.0f)));
    vec3 T = normalize(vec3(Model * vec4(v_tangent, 0.0f)));
    vec3 B = normalize(vec3(Model * vec4(v_bitangent, 0.0f)));
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    B = cross(N, T);
    Geometry.TBN = mat3(T,B,N);
}
void CalculateGeometry(){
    Geometry.N = normalize(Model * vec4(v_normal, 0.0f));      //Surface normal
    Geometry.ViewSpacePosition = ViewModel * vec4(v_position, 1.0f);
    Geometry.WorldSpacePosition = Model * vec4(v_position, 1.0f);
    Geometry.V = -normalize(Geometry.ViewSpacePosition); //Surface to eye direction
    if(dot(Geometry.N,Geometry.V) < 0) Geometry.N = -Geometry.N;
    Geometry.R = normalize(reflect(-Geometry.L,Geometry.N));
    Geometry.H = normalize(Geometry.L+Geometry.V); 
    Geometry.UV = v_uv;
    CalculateTBNMatrix(v_normal);
};
void main()
{
    CalculateGeometry();
    gl_Position = MVP * vec4(v_position, 1.0);
};
