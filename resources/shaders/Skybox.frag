#version 430 core
out vec4 fragment_colour;

in vec3 textureCoordinates;

uniform samplerCube skybox;

void main()
{    
    fragment_colour = texture(skybox, textureCoordinates);
}