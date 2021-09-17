#version 430

layout(quads, equal_spacing, ccw) in;

uniform mat4 mvp_matrix;
layout(binding = 0) uniform sampler2D tex_color;
layout(binding = 1) uniform sampler2D tex_height;
layout(binding = 2) uniform sampler2D tex_normal;

/*--- light stuff----*/
struct PositionalLight
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
};
struct Material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};
uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;
out vec3 varyingVertPos;
out vec3 varyingLightDir;
/*-----------------*/

in vec2 tcs_out[];
out vec2 tes_out;

void main(void)
{
	vec2 tc = vec2(tcs_out[0].x + (gl_TessCoord.x) / 16.0,
				   tcs_out[0].y + (1.0 - gl_TessCoord.y) / 16.0);

	// map the tessellated grid onto the texture rectangle:
	vec4 tessellatedPoint = vec4(gl_in[0].gl_Position.x + gl_TessCoord.x / 16.0, 0.0,
								 gl_in[0].gl_Position.z + gl_TessCoord.y / 16.0, 1.0);

	// add the height from the height map to the vertex:
	tessellatedPoint.y += (texture(tex_height, tc).r) / 20.0;

	gl_Position = mvp_matrix * tessellatedPoint;
	tes_out = tc;

	/*--- light stuff----*/
	// IMPORTANTE DISTINTO
	// para definir las alturas
	// Definimos los elementos de los puntos hacia la camara, como hemos modificado la altura, la transformo en coordenadas del modelo de la camara, al punto lo transformo a coordenadas de la camara con la matrix
	varyingVertPos = (mv_matrix * tessellatedPoint).xyz;
	// de ahi genero un punto desde la camara a aquel punto
	varyingLightDir = light.position - varyingVertPos;
	// LOS PUNTOS VAN A ESTAR ACOMPAÑADOS POR  ATRIBUTOS DE VERTICE, LAS CORDENADAS DE TEXTURA; LA DIRECCION HACIA LA CAMARA Y LA DIRECCION HACIA LA LUZ
}