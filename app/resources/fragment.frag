#version 430 core
out vec4 fragment_colour;

in vec4 something;

void main()
{
    fragment_colour = vec4(0.3) + something * 0.0005;
} 