#version 430 core
#define MAX_LIGHTS 10
#define PI 3.1415926535897932384626433832795

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D hdrBuffer;
layout(binding = 1) uniform sampler2D bloomBuffer;
uniform bool useBloom;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    if(useBloom)
    {
        vec3 bloomColor = texture(bloomBuffer, TexCoords).rgb;
        hdrColor += bloomColor;
    }

    // reinhard
    // vec3 result = hdrColor / (hdrColor + vec3(1.0));
    // exposure
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
    
    // FragColor = vec4(TexCoords, 0.0f, 1.0f) + FragColor*0.0001f;

}