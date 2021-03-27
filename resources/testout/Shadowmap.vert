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
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;
layout (location = 3) in vec3 v_tangent;
layout (location = 4) in vec3 v_bitangent;
void main()
{
    gl_Position = directionalLight.ViewProjection * Model * vec4(v_position, 1.0);
}
