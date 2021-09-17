#version 410
#define PI 3.14159265358
uniform float offset;

mat4 buildRotateX(float degree)
{
    float rad = degree * PI/180.0;
    // in homogeneous coordinates
    mat4 xrot = mat4(1.0,     0.0,       0.0, 0.0,
                    0.0, cos(rad), -sin(rad), 0.0,
                    0.0, sin(rad), cos(rad),  0.0,
                    0.0,      0.0,       0.0, 1.0);
    return xrot;
}

mat4 buildRotateY(float degree)
{
    float rad = degree * PI/180.0;
    // in homogeneous coordinates
    mat4 yrot = mat4(cos(rad), 0.0, sin(rad), 0.0,
                    0.0,       1.0,      0.0, 0.0,
                    -sin(rad), 0.0, cos(rad), 0.0,
                    0.0,       0.0,      0.0, 1.0);
    return yrot;
}


mat4 buildRotateZ(float degree)
{
    float rad = degree * PI/180.0;
    // in homogeneous coordinates
    mat4 zrot = mat4(cos(rad), -sin(rad), 0.0, 0.0,
                     sin(rad),  cos(rad), 0.0, 0.0,
                          0.0,       0.0, 1.0, 0.0,
                          0.0,       0.0, 0.0, 1.0);
    return zrot;
}

void main(void)
{
    mat4 rotatex = buildRotateX(offset);
    mat4 rotatez = buildRotateZ(10 - offset); // no se ve nada cuando esta perpendicular 
    mat4 rotatey = buildRotateY(0.0);

    vec4 myvec;
    if (gl_VertexID == 0)
    {
        myvec = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else if (gl_VertexID == 1)
    {
        myvec = vec4(0.25, 0.0, 0.0, 1.0);
    }
    else if (gl_VertexID == 2)
    {
        myvec = rotatex*vec4(0.25, 0.25, 0.0, 1.0);
    }
    else if (gl_VertexID == 3)
    {
        myvec = rotatex*vec4(0.25, 0.25, 0.0, 1.0);
    }
    else if (gl_VertexID == 4)
    {
        myvec = rotatex*vec4(0.0, 0.25, 0.0, 1.0);
    }
    else
    {
        myvec = vec4(0.0, 0.0, 0.0, 1.0);
    }
     
    gl_Position = rotatez*myvec;
}