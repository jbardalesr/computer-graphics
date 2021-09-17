#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Torus.h"
#include "Utils.h"
using namespace std;

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1
#define numVBOs 12

float cameraX, cameraY, cameraZ;
float lightLocX, lightLocY, lightLocZ;

GLuint torusTexture, torusNormalMap;

GLuint renderingProgram, renderingProgramCubeMap;
GLuint vao[numVAOs], vbo[numVBOs];

float torLocX, torLocY, torLocZ;

GLuint skyboxTexture;

// variable allocation for display
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc;
GLuint mvLoc, color_obj, isTexture, vLoc, projLoc, nLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat;

glm::vec3 currentLightPos;
float lightPos[3];

Torus myTorus(1.0f, 0.4f, 48);
int numTorusVertices, numTorusIndices;
// initial light location

// white light
float globalAmbient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
float lightAmbient[4] = {0.0f, 0.0f, 0.0f, 1.0f};
float lightDiffuse[4] = {0.69020f, 0.52549f, 0.33730f, 1.0f};
float lightSpecular[4] = {0.69020f, 0.52549f, 0.33730f, 1.0f};

// silver material
float *matAmb = Utils::bronzeAmbient();
float *matDif = Utils::bronzeDiffuse();
float *matSpe = Utils::bronzeSpecular();
float matShi = Utils::bronzeShininess();

enum indexObject
{
	SKY_BOX_POSITION = 0,
	TORUS_POSITION = 1,
	TORUS_TEXTURE = 2,
	TORUS_INDICES = 3,
	TORUS_NORMALS = 4,
	TORUS_TANGENTS = 5,
	X_AXIS_POSITION = 6,
	Y_AXIS_POSITION = 7,
	Z_AXIS_POSITION = 8,
	Y_ARROW_POSITION = 9,
};

void setupVertices(void)
{
	float cubeVertexPositions[108] =
		{-1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
		 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
		 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		 -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		 -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
		 -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
		 -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
		 -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f};

	float xAxisPosition[6] = {0.0f, 0.0f, 0.0f,
							  1.0f, 0.0f, 0.0f};

	float yAxisPosition[6] = {0.0f, 0.0f, 0.0f,
							  0.0f, 1.0f, 0.0f};

	float zAxisPosition[6] = {0.0f, 0.0f, 0.0f,
							  0.0f, 0.0f, 1.0f};

	float yAxisArrow[9] = {-0.05f, 1.0f, 0.0f,
						   0.05f, 1.0f, 0.0f,
						   0.0f, 1.1f, 0.0f};

	numTorusVertices = myTorus.getNumVertices();
	numTorusIndices = myTorus.getNumIndices();

	std::vector<int> ind = myTorus.getIndices();
	std::vector<glm::vec3> vert = myTorus.getVertices();
	std::vector<glm::vec2> tex = myTorus.getTexCoords();
	std::vector<glm::vec3> norm = myTorus.getNormals();
	std::vector<glm::vec3> tang = myTorus.getTtangents();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;
	std::vector<float> tanvalues;

	for (int i = 0; i < numTorusVertices; i++)
	{
		pvalues.push_back(vert[i].x);
		pvalues.push_back(vert[i].y);
		pvalues.push_back(vert[i].z);
		tvalues.push_back(tex[i].s);
		tvalues.push_back(tex[i].t);
		nvalues.push_back(norm[i].x);
		nvalues.push_back(norm[i].y);
		nvalues.push_back(norm[i].z);
		tanvalues.push_back((tang[ind[i]]).x);
		tanvalues.push_back((tang[ind[i]]).y);
		tanvalues.push_back((tang[ind[i]]).z);
	}
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SKY_BOX_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexPositions), cubeVertexPositions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TORUS_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TORUS_TEXTURE]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[TORUS_INDICES]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TORUS_NORMALS]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TORUS_TANGENTS]);
	glBufferData(GL_ARRAY_BUFFER, tanvalues.size() * 4, &tanvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[X_AXIS_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xAxisPosition), xAxisPosition, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[Y_AXIS_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(yAxisPosition), yAxisPosition, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[Z_AXIS_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(zAxisPosition), zAxisPosition, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[Y_ARROW_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(yAxisArrow), yAxisArrow, GL_STATIC_DRAW);
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
	renderingProgramCubeMap = Utils::createShaderProgram("cubeShader/vertCShader.glsl", "cubeShader/fragCShader.glsl");

	lightLocX = 3.0f;
	lightLocY = 3.0f;
	lightLocZ = 1.0f;

	torLocX = 0.0f;
	torLocY = 0.0f;
	torLocZ = 0.0f;

	cameraX = 1.5f;
	cameraY = 3.0f;
	cameraZ = 2.5f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices();

	torusTexture = Utils::loadTexture("resources/GoldBricks-ColorMap.png");
	torusNormalMap = Utils::loadTexture("resources/GoldBricks-NormalMap.png");
	skyboxTexture = Utils::loadCubeMap("resources"); // expects a folder name
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void display(GLFWwindow *window, double currentTime)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	vMat = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ),
					   glm::vec3(0.0f, 0.0f, 0.0f),
					   glm::vec3(0.0f, 1.0f, 0.0f));
	// DRAW EL SKY_BOX
	glUseProgram(renderingProgramCubeMap);

	vLoc = glGetUniformLocation(renderingProgramCubeMap, "v_matrix");
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));

	projLoc = glGetUniformLocation(renderingProgramCubeMap, "p_matrix");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SKY_BOX_POSITION]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glActiveTexture(GL_TEXTURE0);
	// una textura en 3D
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW); // cube is CW, but we are viewing the inside
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// para el buffer de profundidad lo habilitamos nuevamente
	glEnable(GL_DEPTH_TEST);

	// DRAW TORUS
	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");
	color_obj = glGetUniformLocation(renderingProgram, "color_obj");
	isTexture = glGetUniformLocation(renderingProgram, "isTexture");

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat = glm::rotate(mMat, toRadians(110.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	mMat = glm::rotate(mMat, toRadians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mvMat = vMat * mMat;

	currentLightPos = glm::vec3(lightLocX, lightLocY, lightLocZ);
	installLights(vMat);

	invTrMat = glm::transpose(glm::inverse(mvMat));

	glUniform1i(isTexture, 1);

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TORUS_POSITION]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TORUS_TEXTURE]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TORUS_NORMALS]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TORUS_TANGENTS]);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, torusNormalMap);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, torusTexture);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glDisable(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, myTorus.getNumVertices());
	glEnable(GL_DEPTH_TEST);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[TORUS_INDICES]);
	glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0);

	glUniform1i(isTexture, 0);
	// DRAW AXIS X
	glUniform4f(color_obj, 0.0f, 1.0f, 0.0f, 1.0f);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[X_AXIS_POSITION]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_LINES, 0, 2);

	// DRAW AXIS Y
	glUniform4f(color_obj, 0.0f, 0.0f, 1.0f, 1.0f);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[Y_AXIS_POSITION]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_LINES, 0, 2);

	// DRAW ARROW Y
	glBindBuffer(GL_ARRAY_BUFFER, vbo[Y_ARROW_POSITION]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 1 * 3);

	// DRAW AXIS Z
	glUniform4f(color_obj, 1.0f, 0.0f, 0.0f, 1.0f);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[Z_AXIS_POSITION]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_LINES, 0, 2);
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
	GLFWwindow *window = glfwCreateWindow(1032, 576, "PC4-1 - Skybox Torus", NULL, NULL);
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