#version 430 core
out vec4 fragment_colour;

in vec4 something;

void main()
{
    fragment_colour = something;
} 