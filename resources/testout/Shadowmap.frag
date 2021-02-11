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
layout(std140, binding=1) uniform InstanceUniforms
{
mat4 Model;
mat4 ViewModel;
mat4 InvT;
mat4 MVP;
};
layout(std140, binding=3) uniform PLight
{
PointLight pointLight;
};
layout(std140, binding=4) uniform DLight
{
DirectionalLight directionalLight;
};
void main()
{
    
}
