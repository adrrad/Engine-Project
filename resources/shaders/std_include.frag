uniform PBRProperties PBR;

in StandardShadingProperties Properties;

layout (location = 0) out vec4 fragment_colour;
layout (location = 1) out vec4 bright_colour;

vec4 CalculateNormalFromMap(vec2 uv)
{
    vec3 normal = texture(NormalMap, uv).xyz;
    // normal.x = normal.x * 2.0 - 1.0;
    // normal.z = normal.z * 2.0 - 1.0;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(Properties.TBN * normal);
    return vec4(normal,0.0f);
}