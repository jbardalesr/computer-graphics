#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL2/SOIL2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <opencv4/opencv2/opencv.hpp>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>			// glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Utils.h"
using namespace std;

Utils::Utils() {}

string Utils::readShaderFile(const char *filePath)
{
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";
	while (!fileStream.eof())
	{
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}

bool Utils::checkOpenGLError()
{
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		cout << "glError: " << glErr << endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

void Utils::printShaderLog(GLuint shader)
{
	int len = 0;
	int chWrittn = 0;
	char *log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = (char *)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		cout << "Shader Info Log: " << log << endl;
		free(log);
	}
}

GLuint Utils::prepareShader(int shaderTYPE, const char *shaderPath)
{
	GLint shaderCompiled;
	string shaderStr = readShaderFile(shaderPath);
	const char *shaderSrc = shaderStr.c_str();
	GLuint shaderRef = glCreateShader(shaderTYPE);
	glShaderSource(shaderRef, 1, &shaderSrc, NULL);
	glCompileShader(shaderRef);
	checkOpenGLError();
	glGetShaderiv(shaderRef, GL_COMPILE_STATUS, &shaderCompiled);
	if (shaderCompiled != 1)
	{
		if (shaderTYPE == 35633)
			cout << "Vertex ";
		if (shaderTYPE == 36488)
			cout << "Tess Control ";
		if (shaderTYPE == 36487)
			cout << "Tess Eval ";
		if (shaderTYPE == 36313)
			cout << "Geometry ";
		if (shaderTYPE == 35632)
			cout << "Fragment ";
		cout << "shader compilation error." << endl;
		printShaderLog(shaderRef);
	}
	return shaderRef;
}

void Utils::printProgramLog(int prog)
{
	int len = 0;
	int chWrittn = 0;
	char *log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = (char *)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		cout << "Program Info Log: " << log << endl;
		free(log);
	}
}

int Utils::finalizeShaderProgram(GLuint sprogram)
{
	GLint linked;
	glLinkProgram(sprogram);
	checkOpenGLError();
	glGetProgramiv(sprogram, GL_LINK_STATUS, &linked);
	if (linked != 1)
	{
		cout << "linking failed" << endl;
		printProgramLog(sprogram);
	}
	return sprogram;
}

GLuint Utils::createShaderProgram(const char *vp, const char *fp)
{
	GLuint vShader = prepareShader(GL_VERTEX_SHADER, vp);
	GLuint fShader = prepareShader(GL_FRAGMENT_SHADER, fp);
	GLuint vfprogram = glCreateProgram();
	glAttachShader(vfprogram, vShader);
	glAttachShader(vfprogram, fShader);
	finalizeShaderProgram(vfprogram);
	return vfprogram;
}

GLuint Utils::createShaderProgram(const char *vp, const char *gp, const char *fp)
{
	GLuint vShader = prepareShader(GL_VERTEX_SHADER, vp);
	GLuint gShader = prepareShader(GL_GEOMETRY_SHADER, gp);
	GLuint fShader = prepareShader(GL_FRAGMENT_SHADER, fp);
	GLuint vgfprogram = glCreateProgram();
	glAttachShader(vgfprogram, vShader);
	glAttachShader(vgfprogram, gShader);
	glAttachShader(vgfprogram, fShader);
	finalizeShaderProgram(vgfprogram);
	return vgfprogram;
}

GLuint Utils::createShaderProgram(const char *vp, const char *tCS, const char *tES, const char *fp)
{
	GLuint vShader = prepareShader(GL_VERTEX_SHADER, vp);
	GLuint tcShader = prepareShader(GL_TESS_CONTROL_SHADER, tCS);
	GLuint teShader = prepareShader(GL_TESS_EVALUATION_SHADER, tES);
	GLuint fShader = prepareShader(GL_FRAGMENT_SHADER, fp);
	GLuint vtfprogram = glCreateProgram();
	glAttachShader(vtfprogram, vShader);
	glAttachShader(vtfprogram, tcShader);
	glAttachShader(vtfprogram, teShader);
	glAttachShader(vtfprogram, fShader);
	finalizeShaderProgram(vtfprogram);
	return vtfprogram;
}

GLuint Utils::createShaderProgram(const char *vp, const char *tCS, const char *tES, char *gp, const char *fp)
{
	GLuint vShader = prepareShader(GL_VERTEX_SHADER, vp);
	GLuint tcShader = prepareShader(GL_TESS_CONTROL_SHADER, tCS);
	GLuint teShader = prepareShader(GL_TESS_EVALUATION_SHADER, tES);
	GLuint gShader = prepareShader(GL_GEOMETRY_SHADER, gp);
	GLuint fShader = prepareShader(GL_FRAGMENT_SHADER, fp);
	GLuint vtgfprogram = glCreateProgram();
	glAttachShader(vtgfprogram, vShader);
	glAttachShader(vtgfprogram, tcShader);
	glAttachShader(vtgfprogram, teShader);
	glAttachShader(vtgfprogram, gShader);
	glAttachShader(vtgfprogram, fShader);
	finalizeShaderProgram(vtgfprogram);
	return vtgfprogram;
}

GLuint Utils::loadTexture(const char *texImagePath)
{
	GLuint brickTexture;
	// leo la informacion y lo almaceno en la matriz
	cv::Mat image = cv::imread(texImagePath);

	if (image.empty())
	{
		std::cout << "image empty" << std::endl;
	}
	else
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		// generamos un identificador para una textura asi como vbo
		glGenTextures(1, &brickTexture);

		glBindTexture(GL_TEXTURE_2D, brickTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		if (CV_VERSION_MAJOR >= 4)
			cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
		else
			cout << "Su version de OpenGL < 4, cambie COLOR_BGR2RGB por CV_BGR2RGB en utils.h:42" << endl;

		glTexImage2D(GL_TEXTURE_2D,
					 0,				   // nivel de los maps para el mipmapping
					 GL_RGB,		   // color
					 image.cols,	   // alto de la imagen
					 image.rows,	   // ancho de la imagen
					 0,				   // lee toda la imagen, que no me falte pixeles.
					 GL_RGB,		   // la imagen que te voy a dar esta en formato bgr no es rgb
					 GL_UNSIGNED_BYTE, // tipo de dato de imagen
					 image.ptr());	   // puntero de la imagen

		glGenerateMipmap(GL_TEXTURE_2D);

		// APLICANDO EL FILTRO ANISOTROPICO
		if (glewIsSupported("GL_EXT_texture_filter_anisotropic"))
		{
			GLfloat anisoset = 0.0f; // nivel de suavizado 1.0 ISOTROPICO
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoset);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoset);
		}
	}
	return brickTexture;
}

// GOLD material - ambient, diffuse, specular, and shininess
float *Utils::goldAmbient()
{
	static float a[4] = {0.2473f, 0.1995f, 0.0745f, 1};
	return (float *)a;
}
float *Utils::goldDiffuse()
{
	static float a[4] = {0.7516f, 0.6065f, 0.2265f, 1};
	return (float *)a;
}
float *Utils::goldSpecular()
{
	static float a[4] = {0.6283f, 0.5559f, 0.3661f, 1};
	return (float *)a;
}
float Utils::goldShininess() { return 51.2f; }

// SILVER material - ambient, diffuse, specular, and shininess
float *Utils::silverAmbient()
{
	static float a[4] = {0.1923f, 0.1923f, 0.1923f, 1};
	return (float *)a;
}
float *Utils::silverDiffuse()
{
	static float a[4] = {0.5075f, 0.5075f, 0.5075f, 1};
	return (float *)a;
}
float *Utils::silverSpecular()
{
	static float a[4] = {0.5083f, 0.5083f, 0.5083f, 1};
	return (float *)a;
}
float Utils::silverShininess() { return 51.2f; }

// BRONZE material - ambient, diffuse, specular, and shininess
float *Utils::bronzeAmbient()
{
	static float a[4] = {0.2125f, 0.1275f, 0.0540f, 1};
	return (float *)a;
}
float *Utils::bronzeDiffuse()
{
	static float a[4] = {0.7140f, 0.4284f, 0.1814f, 1};
	return (float *)a;
}
float *Utils::bronzeSpecular()
{
	static float a[4] = {0.3936f, 0.2719f, 0.1667f, 1};
	return (float *)a;
}
float Utils::bronzeShininess() { return 25.6f; }
