#version 410
#define M_PI 3.14159265358
void main(void){
    float x = gl_VertexID*0.20;
    float sinx = 0.5*sin(x*M_PI*2);

    gl_Position = vec4(x - 1.0, sinx, 0.0,1.0);

}