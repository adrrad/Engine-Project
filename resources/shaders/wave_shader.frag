#version 430 core
out vec4 fragment_colour;

in vec4 something;
in vec4 colour;
void main()
{
    fragment_colour = colour + something * 0.0001;
} 