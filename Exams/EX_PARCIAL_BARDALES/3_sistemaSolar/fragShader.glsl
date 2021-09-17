#version 430

// in vec4 varyingColor;
out vec4 color;
in vec2 tc;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
layout (binding=0) uniform sampler2D s;

void main(void)
{	
    // color = varyingColor;
    color = texture(s, tc);
}
