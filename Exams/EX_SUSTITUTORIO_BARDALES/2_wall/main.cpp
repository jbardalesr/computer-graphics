#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Utils.h"
using namespace std;

float toRadians(float degrees) { return (degrees * 3.14159f) / 180.0f; }

#define numVAOs 1
#define numVBOs 6

float cameraX, cameraY, cameraZ;
float wallLocX, wallLocY, wallLocZ;
float lightLocX, lightLocY, lightLocZ;

GLuint wallNormalMap;
GLuint wallTexture;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// variable allocation for display
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc;
GLuint mvLoc, projLoc, nLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat;
glm::vec3 currentLightPos;
float lightPos[3];

// white light
float globalAmbient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
float lightAmbient[4] = {0.0f, 0.0f, 0.0f, 1.0f};
float lightDiffuse[4] = {01.0, 1.0, 1.0, 1.0f};
float lightSpecular[4] = {1.0f, 1.0, 1.0, 1.0f};

// silver material
float *matAmb = Utils::bronzeAmbient();
float *matDif = Utils::bronzeDiffuse();
float *matSpe = Utils::bronzeSpecular();
float matShi = Utils::bronzeShininess();

enum IndexObject
{
	WALL_POSITION = 0,
	WALL_TEXTURE = 1,
	WALL_NORMAL = 2,
	WALL_TANGENTS = 3
};

void setupVertices(void)
{
	// wall
	float wallPosition[18] = {-1.0, 0.0, -1.0,
							  1.0, 0.0, 1.0,
							  -1.0, 0.0, 1.0,
							  -1.0, 0.0, -1.0,
							  1.0, 0.0, 1.0,
							  1.0, 0.0, -1.0};

	// texel pixel correspondiente de una textura.
	float wallTextureMap[12] = {0.0, 1.0,
								1.0, 0.0,
								0.0, 0.0,
								0.0, 1.0,
								1.0, 0.0,
								1.0, 1.0};

	float wallNormal[18] = {0.0, 1.0, 0.0,
							0.0, 1.0, 0.0,
							0.0, 1.0, 0.0,
							0.0, 1.0, 0.0,
							0.0, 1.0, 0.0,
							0.0, 1.0, 0.0};

	float wallTangents[18] = {1.0, 0.0, 1.0,
							  1.0, 0.0, 1.0,
							  1.0, 0.0, 1.0,
							  1.0, 0.0, 1.0,
							  1.0, 0.0, 1.0,
							  1.0, 0.0, 1.0};

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[WALL_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallPosition), &wallPosition[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[WALL_TEXTURE]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallTextureMap), &wallTextureMap[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[WALL_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallNormal), &wallNormal[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[WALL_TANGENTS]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallTangents), &wallTangents[0], GL_STATIC_DRAW);
}

void installLights(glm::mat4 v_matrix)
{
	glm::vec3 transformed = glm::vec3(v_matrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	// get the locations of the light and material fields in the shader
	globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
	ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
	diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
	specLoc = glGetUniformLocation(renderingProgram, "light.specular");
	posLoc = glGetUniformLocation(renderingProgram, "light.position");
	mambLoc = glGetUniformLocation(renderingProgram, "material.ambient");
	mdiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
	mspecLoc = glGetUniformLocation(renderingProgram, "material.specular");
	mshiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
	glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);
	glProgramUniform4fv(renderingProgram, mambLoc, 1, matAmb);
	glProgramUniform4fv(renderingProgram, mdiffLoc, 1, matDif);
	glProgramUniform4fv(renderingProgram, mspecLoc, 1, matSpe);
	glProgramUniform1f(renderingProgram, mshiLoc, matShi);
}

void init(GLFWwindow *window)
{
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

	cameraX = -2.0f;
	cameraY = 0.0f;
	cameraZ = 1.5f;

	wallLocX = 0.0f;
	wallLocY = 0.0f;
	wallLocZ = 0.0f;

	lightLocX = 0.2f;
	lightLocY = 0.2f;
	lightLocZ = 0.2f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices();

	wallTexture = Utils::loadTexture("brickwall.jpg");
	wallNormalMap = Utils::loadTexture("normal.jpg");
}

void display(GLFWwindow *window, double currentTime)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");

	vMat = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ),
					   glm::vec3(0.0f, 0.0f, 0.0f),
					   glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(wallLocX, wallLocY, wallLocZ));
	mMat = glm::rotate(mMat, toRadians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mMat = glm::rotate(mMat, toRadians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	currentLightPos = glm::vec3(lightLocX, lightLocY, lightLocZ);
	installLights(vMat);

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, wallNormalMap);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, wallTexture);

	// glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glDrawArrays(GL_TRIANGLES, 0, 6);
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
	GLFWwindow *window = glfwCreateWindow(800, 800, "Chapter10 - program3", NULL, NULL);
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