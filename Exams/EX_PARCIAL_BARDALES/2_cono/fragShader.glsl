#version 430

out vec4 color;
uniform vec4 color_custom;

void main(void)
{
    color = color_custom;
}