#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Cone.h"
#include "Utils.h"

using namespace std;

#define numVAOs 1
#define numVBOs 2
#define prec 20

float cameraX, cameraY, cameraZ;
float coneX, coneY, coneZ;
float aspect;

GLuint renderingProgram;
GLuint vao[numVAOs], vbo[numVBOs];
GLuint dogeTexture;
GLuint mvLoc, projLoc, color;

int width, height;
glm::mat4 pMat, vMat, mMat, mvMat, view;
Cone myCone = Cone(prec);

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

void setupVertices(void)
{
	std::vector<int> ind = myCone.getIndices();
	std::vector<glm::vec3> vert = myCone.getVertices();
	std::vector<glm::vec2> tex = myCone.getTexCoords();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> bvalues;
	std::vector<float> nvalues;

	int numIndices = myCone.getNumIndices();
	for (int i = 0; i < numIndices; i++)
	{
		pvalues.push_back((vert[ind[i]]).x);
		pvalues.push_back((vert[ind[i]]).y);
		pvalues.push_back((vert[ind[i]]).z);

		tvalues.push_back((tex[ind[i]]).s);
		tvalues.push_back((tex[ind[i]]).t);
	}

	bvalues.push_back(0);
	bvalues.push_back(0);
	bvalues.push_back(0);

	for (int i = 0; i <= prec; i++)
	{
		float phi = toRadians(i * 360.0f / (float)prec); // 0 <= φ <= 2π
		float z = cos(phi);
		float x = sin(phi);

		bvalues.push_back(x);
		bvalues.push_back(0);
		bvalues.push_back(z);
	}

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, bvalues.size() * 4, &bvalues[0], GL_STATIC_DRAW);
}

void init(GLFWwindow *window)
{
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

	cameraX = 0.0f;
	cameraY = -2.0f;
	cameraZ = 5.0f;

	coneX = 0.0f;
	coneY = 0.0f;
	coneZ = 0.0f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices();
}

void display(GLFWwindow *window, double currentTime)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	// etiquetas unforms
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	color = glGetUniformLocation(renderingProgram, "color_custom");

	const float radius = 5.0f;
	// la camara en cada momento esta en ubicación distinta, currentTime desde que aparece la ventana para obtener un angulo que aumenta
	float camX = sin(currentTime) * radius;
	float camZ = cos(currentTime) * radius;
	// transforma de coordenadas mundiales a camara
	view = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), // ubicado en este lugar siguiendo una trayectoria circular
					   glm::vec3(0.0f, 0.0f, 0.0f),	   // mira al cero
					   glm::vec3(0.0f, 1.0f, 0.0f));

	//initial compute
	vMat = view;

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(coneX, coneY, coneZ));
	mvMat = vMat * mMat;

	// send uniforms
	glUniform4f(color, 0.6431f, 0.8314f, 0.2157f, 1.0f);

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, myCone.getNumIndices());

	// TAPA 1
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	mvMat = vMat * mMat;

	glUniform4f(color, 0.2549f, 0.3333f, 0.062745f, 1.0f);
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLE_FAN, 0, (12.0f * 3.0f + 1.0f));
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
	GLFWwindow *window = glfwCreateWindow(600, 600, "Test 2 - problem 3", NULL, NULL);
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