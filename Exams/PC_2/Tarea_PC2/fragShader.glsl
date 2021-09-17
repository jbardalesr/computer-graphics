#version 410

out vec4 color;
uniform vec4 color_cube;

void main(void)
{
    color = color_cube;
}