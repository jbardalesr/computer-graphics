#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Utils.h"
#include <stack>
#include "Sphere.h"

using namespace std;

void passOne(void);
void passTwo(void);

#define numVAOs 1
#define numVBOs 20

float aspect;

GLuint vLoc, projLoc;
GLuint renderingProgram1, renderingProgram2;
GLuint vao[numVAOs], vbo[numVBOs];
GLuint mvLoc, nLoc, colorObj;

int width, height;

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

// LUZ
// locations for shader uniform variables
GLuint globalAmbLoc,

	ambLoc,
	diffLoc,
	specLoc,
	locationLoc,
	dirLoc,
	cutoffLoc,
	expLoc,

	mAmbLoc,
	mDiffLoc,
	mSpecLoc,
	mShiLoc,

	sLoc;

glm::mat4 pMat, vMat, mMat, mvMat, invTrMat, view;

// light position as Vector3f, in both model and view space
glm::vec3 transformed;

// light position as float array
float lightPos[3];

// initial light location
// white light properties
float globalAmbient[4] = {0.1f, 0.1f, 0.1f, 1.0f};

float spotLightAmbient[4] = {0.0f, 0.0f, 0.0f, 1.0f};
float spotLightDiffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float spotLightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
glm::vec3 spotLightLocation = glm::vec3(-5.0f, 5.0f, 2.0f);
float spotLightDirection[3] = {-5.0f, 3.0f, 4.0f};
float spotLightCutoff = glm::cos(toRadians(17.0f));
float spotLightExponent = 10.0f;
void installLights(glm::mat4 vMatrix);
// gold material properties
float *matAmb = Utils::silverAmbient();
float *matDif = Utils::silverDiffuse();
float *matSpe = Utils::silverSpecular();
float matShi = Utils::silverShininess();

glm::vec3 planeLoc(0.0f, 0.0f, 0.0f);
glm::vec3 sphereLoc(0.0f, 1.0f, 0.0f);
glm::vec3 cameraLoc(0.0f, 2.0f, 5.0f);

stack<glm::mat4> mvStack;

Sphere mySphere = Sphere(48);

enum indexObject
{
	PLANE_POSITION = 0,
	PLANE_NORMAL = 1,
	SPHERE_POSITION = 2,
	SPHERE_TEXTURE = 3,
	SPHERE_NORMALS = 4,
	SPHERE_TANGENTS = 5,
};

// shadow stuff
int scSizeX, scSizeY;
GLuint shadowTex, shadowBuffer;
glm::mat4 lightVmatrix;
glm::mat4 lightPmatrix;
glm::mat4 shadowMVP1;
glm::mat4 shadowMVP2;
glm::mat4 b;

void setupVertices(void)
{

	// PLANO
	float planePosition[18] = {-7.0, 0.0, -7.0,
							   7.0, 0.0, 7.0,
							   -7.0, 0.0, 7.0,
							   -7.0, 0.0, -7.0,
							   7.0, 0.0, 7.0,
							   7.0, 0.0, -7.0};

	// PLANO NORMAL
	float planeNormal[18] = {0.0, 1.0, 0.0,
							 0.0, 1.0, 0.0,
							 0.0, 1.0, 0.0,
							 0.0, 1.0, 0.0,
							 0.0, 1.0, 0.0,
							 0.0, 1.0, 0.0};

	// no confundir indices de la esfera con los vertices de la esfera, ver la esfera con triangulos
	std::vector<int> ind = mySphere.getIndices();
	std::vector<glm::vec3> vert = mySphere.getVertices();
	std::vector<glm::vec2> tex = mySphere.getTexCoords();
	std::vector<glm::vec3> norm = mySphere.getNormals();
	std::vector<glm::vec3> tang = mySphere.getTangents();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;
	std::vector<float> tanvalues;

	for (int i = 0; i < mySphere.getNumIndices(); i++)
	{
		pvalues.push_back((vert[ind[i]]).x);
		pvalues.push_back((vert[ind[i]]).y);
		pvalues.push_back((vert[ind[i]]).z);
		tvalues.push_back((tex[ind[i]]).s);
		tvalues.push_back((tex[ind[i]]).t);
		nvalues.push_back((norm[ind[i]]).x);
		nvalues.push_back((norm[ind[i]]).y);
		nvalues.push_back((norm[ind[i]]).z);
		tanvalues.push_back((tang[ind[i]]).x);
		tanvalues.push_back((tang[ind[i]]).y);
		tanvalues.push_back((tang[ind[i]]).z);
	}

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[PLANE_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planePosition), &planePosition[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[PLANE_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeNormal), &planeNormal[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_TEXTURE]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_NORMALS]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_TANGENTS]);
	glBufferData(GL_ARRAY_BUFFER, tanvalues.size() * 4, &tanvalues[0], GL_STATIC_DRAW);
}

void setupShadowBuffers(GLFWwindow *window)
{
	glfwGetFramebufferSize(window, &width, &height);
	scSizeX = width;
	scSizeY = height;

	glGenFramebuffers(1, &shadowBuffer);

	// definimos la textura de la sombra.
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, scSizeX, scSizeY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// may reduce shadow border artifacts
	// cuando pida coordenadas fuera del rango que sean las mismas que el borde
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void passOne(void)
{
	glUseProgram(renderingProgram1);

	// draw the PLANE
	mMat = glm::translate(glm::mat4(1.0f), planeLoc);

	shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
	sLoc = glGetUniformLocation(renderingProgram1, "shadowMVP");
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glClear(GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[PLANE_POSITION]);
	glDrawArrays(GL_TRIANGLES, 0, 2 * 3);

	// draw the SPHERE
	mMat = glm::translate(glm::mat4(1.0f), sphereLoc);
	shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_POSITION]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
}

void passTwo(void)
{
	glUseProgram(renderingProgram2);

	view = glm::lookAt(glm::vec3(cameraLoc.x, cameraLoc.y, cameraLoc.z),
					   glm::vec3(0.0f, 0.0f, 0.0f),
					   glm::vec3(0.0f, 1.0f, 0.0f));

	vMat = view;

	// DRAW SCENE
	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram2, "norm_matrix");
	colorObj = glGetUniformLocation(renderingProgram2, "color_obj");
	sLoc = glGetUniformLocation(renderingProgram2, "shadowMVP");

	//////////////////////////////////////////////////////////////////
	mvStack.push(vMat);
	{
		// PLANE
		mvStack.push(mvStack.top());
		{

			installLights(vMat);
			mvStack.top() *= glm::translate(glm::mat4(1.0f), planeLoc);
			invTrMat = glm::transpose(glm::inverse(mvStack.top()));
			shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;

			glUniform4f(colorObj, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
			glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
			glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
			glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

			glBindBuffer(GL_ARRAY_BUFFER, vbo[PLANE_POSITION]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);

			// contiene los normales
			glBindBuffer(GL_ARRAY_BUFFER, vbo[PLANE_NORMAL]);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDrawArrays(GL_TRIANGLES, 0, 3 * 6);
		}
		mvStack.pop();

		// SPHERE
		mvStack.push(mvStack.top());
		{
			installLights(vMat);
			mvStack.top() *= glm::translate(glm::mat4(1.0f), sphereLoc);
			invTrMat = glm::transpose(glm::inverse(mvStack.top()));
			shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;

			glUniform4f(colorObj, 0.20392, 0.560784, 0.78823f, 1.0f);
			glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
			glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
			glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

			glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_POSITION]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);

			// contiene los normales
			glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_NORMALS]);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);

			//glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
		}
		mvStack.pop();
	}
	mvStack.pop();
}

void installLights(glm::mat4 vMatrix)
{
	// convert light's position to view space, and save it in a float array
	transformed = glm::vec3(vMatrix * glm::vec4(spotLightLocation, 1.0));
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	// get the locations of the light and material fields in the shader
	globalAmbLoc = glGetUniformLocation(renderingProgram2, "globalAmbient");

	ambLoc = glGetUniformLocation(renderingProgram2, "light.ambient");
	diffLoc = glGetUniformLocation(renderingProgram2, "light.diffuse");
	specLoc = glGetUniformLocation(renderingProgram2, "light.specular");
	locationLoc = glGetUniformLocation(renderingProgram2, "light.location");
	dirLoc = glGetUniformLocation(renderingProgram2, "light.direction");
	cutoffLoc = glGetUniformLocation(renderingProgram2, "light.cutoff");
	expLoc = glGetUniformLocation(renderingProgram2, "light.exponent");

	mAmbLoc = glGetUniformLocation(renderingProgram2, "material.ambient");
	mDiffLoc = glGetUniformLocation(renderingProgram2, "material.diffuse");
	mSpecLoc = glGetUniformLocation(renderingProgram2, "material.specular");
	mShiLoc = glGetUniformLocation(renderingProgram2, "material.shininess");

	// set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram2, globalAmbLoc, 1, globalAmbient);

	glProgramUniform4fv(renderingProgram2, ambLoc, 1, spotLightAmbient);
	glProgramUniform4fv(renderingProgram2, diffLoc, 1, spotLightDiffuse);
	glProgramUniform4fv(renderingProgram2, specLoc, 1, spotLightSpecular);
	glProgramUniform3fv(renderingProgram2, locationLoc, 1, lightPos);

	glProgramUniform3fv(renderingProgram2, dirLoc, 1, spotLightDirection);
	glProgramUniform1f(renderingProgram2, cutoffLoc, spotLightCutoff);
	glProgramUniform1f(renderingProgram2, expLoc, spotLightExponent);

	glProgramUniform4fv(renderingProgram2, mAmbLoc, 1, matAmb);
	glProgramUniform4fv(renderingProgram2, mDiffLoc, 1, matDif);
	glProgramUniform4fv(renderingProgram2, mSpecLoc, 1, matSpe);
	glProgramUniform1f(renderingProgram2, mShiLoc, matShi);
}

void init(GLFWwindow *window)
{
	renderingProgram1 = Utils::createShaderProgram("./vert1shader.glsl", "./frag1shader.glsl");
	renderingProgram2 = Utils::createShaderProgram("./BlinnPhongShaders/vertShader.glsl", "./BlinnPhongShaders/fragShader.glsl");

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices();
	setupShadowBuffers(window);

	// desde 0 a 1, b se utiliza para llevar del sistema coordeno de la textura
	b = glm::mat4(0.5f, 0.0f, 0.0f, 0.0f,
				  0.0f, 0.5f, 0.0f, 0.0f,
				  0.0f, 0.0f, 0.5f, 0.0f,
				  0.5f, 0.5f, 0.5f, 1.0f);
}

void display(GLFWwindow *window, double currentTime)
{
	// llenamos el buffer con (1, 1, 1)
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	glm::vec3 origin(0.0f, 0.0f, 0.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	lightVmatrix = glm::lookAt(spotLightLocation, origin, up);
	lightPmatrix = glm::perspective(toRadians(60.0f), aspect, 0.1f, 1000.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTex, 0);

	glDrawBuffer(GL_NONE);
	glEnable(GL_DEPTH_TEST);
	// para evitar el acne de sombra
	glEnable(GL_POLYGON_OFFSET_FILL); // for reducing
	glPolygonOffset(2.0f, 4.0f);	  //  shadow artifacts

	passOne();

	// para evitar el acne de sombra
	glDisable(GL_POLYGON_OFFSET_FILL); // artifact reduction, continued

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	glDrawBuffer(GL_FRONT);

	passTwo();
}

void window_size_callback(GLFWwindow *win, int newWidth, int newHeight)
{
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

int main(void)
{
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow *window = glfwCreateWindow(1000, 640, "PC4-2 - Sphere Shadow", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK)
	{
		exit(EXIT_FAILURE);
	}
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init(window);

	while (!glfwWindowShouldClose(window))
	{
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
