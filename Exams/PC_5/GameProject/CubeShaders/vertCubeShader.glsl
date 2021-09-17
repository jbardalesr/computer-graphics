#version 430
// las posiciones que se cojen del vbo son atributo de vertice que seran la coordenada despues.
layout (location = 0) in vec3 position;
// salida que se recogerá en el shader
out vec3 tc;

uniform mat4 v_matrix;
uniform mat4 p_matrix;
layout (binding = 0) uniform samplerCube samp;

void main(void)
{
    // coordenadas en 3D del cubo textura, en el caso l, se le dio una coordenada en 2D pero hay errores de costura
    tc = position;
    // matrix en que la traslacion es 0
    mat4 v3_matrix = mat4(mat3(v_matrix));
    gl_Position = p_matrix * v3_matrix * vec4(position, 1.0);
}