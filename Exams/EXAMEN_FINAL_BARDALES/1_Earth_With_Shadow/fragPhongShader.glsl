#version 430

in vec3 varyingLightDir;
in vec3 varyingVertPos;
in vec3 varyingNormal;
in vec3 varyingTangent;

in vec3 originalVertex;
in vec2 tc;

// shadow
out vec4 fragColor;
in vec4 shadow_coord;


layout (binding=0) uniform sampler2D s;
layout (binding=1) uniform sampler2D t;

struct PositionalLight
{	vec4 ambient;  
	vec4 diffuse;  
	vec4 specular;  
	vec3 position;
};

struct Material
{	vec4 ambient;  
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

vec3 calcNewNormal()
{
	vec3 normal = normalize(varyingNormal);
	vec3 tangent = normalize(varyingTangent);
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 bitangent = cross(tangent, normal);
	mat3 tbn = mat3(tangent, bitangent, normal);
	vec3 retrievedNormal = texture(s, tc).xyz;
	retrievedNormal = retrievedNormal * 2.0 - 1.0;
	vec3 newNormal = tbn * retrievedNormal;
	newNormal = normalize(newNormal);
	return newNormal;
}

uniform mat4 shadowMVP;
layout (binding=2) uniform sampler2DShadow shadowTex;

float lookup(float x, float y)
{  	
	// determina si en es punto hay sombra para realizar el promedio, ver imagen
	// -0.01 el componente z sirve para darle esa profundidad de sombra
	// a shadow_coord le adicionamos quiero que me diga si en ese punto que estoy tratando de crear si es sombra o no.
	// si usamos 64 tenemos una sombra mas adecuada
	float t = textureProj(shadowTex, shadow_coord + vec4(x * 0.001 * shadow_coord.w,
                                                         y * 0.001 * shadow_coord.w,
                                                         -0.01, 0.0));
	return t;
}

void main(void)
{	
    float shadowFactor = 0.0;
	
	// normalize the light, normal, and view vectors:
	vec3 L = normalize(varyingLightDir);
	vec3 V = normalize(-varyingVertPos);
	vec3 N = calcNewNormal();

	float swidth = 4.0;
	// consigo la coordenada del fragmento, gl_FragCord coordenada del fragmento, en la ventana de 600x600
	vec2 o = mod(floor(gl_FragCoord.xy), 2.0) * swidth;
	shadowFactor += lookup(-1.5*swidth + o.x,  1.5*swidth - o.y);
	shadowFactor += lookup(-1.5*swidth + o.x, -0.5*swidth - o.y);
	shadowFactor += lookup( 0.5*swidth + o.x,  1.5*swidth - o.y);
	shadowFactor += lookup( 0.5*swidth + o.x, -0.5*swidth - o.y);
	// 
	shadowFactor = shadowFactor / 4.0;

	// get the angle between the light and surface normal:
	float cosTheta = dot(L,N);
	// compute light reflection vector, with respect N:
	vec3 R = normalize(reflect(-L, N));
	// angle between the view vector and reflected light:
	float cosPhi = dot(V,R);
	vec4 texC = texture(t,tc);
	
	// compute ADS contributions with surface texture image:
    vec4 shadowColor = globalAmbient * material.ambient + light.ambient * material.ambient;
	vec4 lightedColor = globalAmbient + light.ambient * texC
	+ light.diffuse * texC * max(cosTheta,0.0)
	+ light.specular * texC * pow(max(cosPhi,0.0), material.shininess);

    //fragColor = lightedColor;
   	fragColor = vec4((shadowColor.xyz + shadowFactor*(lightedColor.xyz)),1.0);
}
