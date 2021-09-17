#version 430

in vec2 tc;
out vec4 color;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
// la imagen se guarda en un sampler2D que es una imagen en 2D
// binding=0, glTexture0
layout (binding=0) uniform sampler2D samp; 

void main(void)
{	
    // hace el proceso de textura, el color retorna el color del fragmento.
    color = texture(samp, tc);
}
