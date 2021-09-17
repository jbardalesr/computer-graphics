#version 430

layout (location=0) in vec3 position;
layout (location=1) in vec2 texCoord;

uniform mat4 v_matrix;
uniform mat4 proj_matrix;
uniform float tf;
uniform float random;

//out vec4 varyingColor;
out vec2 tc;

mat4 buildRotateX(float rad);
mat4 buildRotateY(float rad);
mat4 buildRotateZ(float rad);
mat4 buildTranslate(float x, float y, float z);

void main(void)
{	
	float i = gl_InstanceID + tf + random;
	
	float a = cos(0.18*i)*15.0;
    float b = sin(0.58*i)*15.0;
    float c = sin(i)*15.0;
    
    mat4 localRotX = buildRotateX(0.5*i);
    mat4 localRotY = buildRotateX(0.8*i);
    mat4 localRotZ = buildRotateX(1.7*i);

	mat4 localTrans = buildTranslate(a, b, c);

	mat4 newM_matrix = localTrans * localRotX * localRotY * localRotZ;
	mat4 mv_matrix = v_matrix * newM_matrix;

	gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
	tc = texCoord;
}

mat4 buildTranslate(float x, float y, float z)
{	mat4 trans = mat4(1.0, 0.0, 0.0, 0.0,
                    0.0, 1.0, 0.0, 0.0,
                    0.0, 0.0, 1.0, 0.0,
                    x, y, z, 1.0 );
	return trans;
}

//  rotation around the X axis
mat4 buildRotateX(float rad)
{	mat4 xrot = mat4(1.0, 0.0, 0.0, 0.0,
                    0.0, cos(rad), -sin(rad), 0.0,
                    0.0, sin(rad), cos(rad), 0.0,
                    0.0, 0.0, 0.0, 1.0 );
	return xrot;
}

//  rotation around the Y axis
mat4 buildRotateY(float rad)
{	mat4 yrot = mat4(cos(rad), 0.0, sin(rad), 0.0,
                    0.0, 1.0, 0.0, 0.0,
                    -sin(rad), 0.0, cos(rad), 0.0,
                    0.0, 0.0, 0.0, 1.0 );
	return yrot;
}

//  rotation around the Z axis
mat4 buildRotateZ(float rad)
{	mat4 zrot = mat4(cos(rad), sin(rad), 0.0, 0.0,
                    -sin(rad), cos(rad), 0.0, 0.0,
                    0.0, 0.0, 1.0, 0.0,
                    0.0, 0.0, 0.0, 1.0 );
	return zrot;
}
