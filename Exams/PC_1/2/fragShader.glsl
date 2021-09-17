#version 410
out vec4 color;
layout (location = 0) in vec4 vertex;
void main(void){
    color = vertex;
}