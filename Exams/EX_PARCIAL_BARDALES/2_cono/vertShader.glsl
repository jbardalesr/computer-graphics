#version 430

layout (location = 0) in vec3 position;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

mat4 buildRotateX(float rad);
mat4 buildRotateY(float rad);
mat4 buildRotateZ(float rad);
mat4 buildTranslate(float x, float y, float z);

void main(void)
{
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}