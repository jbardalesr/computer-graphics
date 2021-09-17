#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>			// glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Utils.h"
#include "Sphere.h"
using namespace std;

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1
#define numVBOs 4

float cameraX, cameraY, cameraZ;
float lightLocX, lightLocY, lightLocZ;

GLuint earthTexture, moonTexture;
GLuint moonNormalMap, earthNormalMap;

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

// pila de matrices 4x4
stack<glm::mat4> mvStack;
Sphere mySphere = Sphere(48);

// white light
float globalAmbient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
float lightAmbient[4] = {0.0f, 0.0f, 0.0f, 1.0f};
float lightDiffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float lightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};

// silver material
float *matAmb = Utils::silverAmbient();
float *matDif = Utils::silverDiffuse();
float *matSpe = Utils::silverSpecular();
float matShi = Utils::silverShininess();

void setupVertices(void)
{
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

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, tanvalues.size() * 4, &tanvalues[0], GL_STATIC_DRAW);
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

	lightLocX = 3.0f;
	lightLocY = 2.0f;
	lightLocZ = 3.0f;

	cameraX = 0.0f;
	cameraY = 6.0f;
	cameraZ = 12.0f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices();

	earthTexture = Utils::loadTexture("img/earth.jpg");
	earthNormalMap = Utils::loadTexture("img/earthMAP.png");
	moonTexture = Utils::loadTexture("img/moon.jpg");
	moonNormalMap = Utils::loadTexture("img/moonNORMAL.jpg");
}

void display(GLFWwindow *window, double currentTime)
{
	// limpiamos el buffer
	float radius = 6.0f;
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	// definimos dos variables uniformes
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");

	// Matriz transfoma las coordenadas mundo a cooordenas cámara.
	vMat = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), // ubicado en este lugar siguiendo una trayectoria circular
					   glm::vec3(0.0f, 0.0f, 0.0f),			 // mira al cero
					   glm::vec3(0.0f, 1.0f, 0.0f));

	currentLightPos = glm::vec3(lightLocX, lightLocY, lightLocZ);
	installLights(vMat);

	mvStack.push(vMat);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	//////////// TIERRA
	mvStack.push(mvStack.top());
	{
		mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime / 3, glm::vec3(0.0, 1.0, 0.0));
		mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0f, 0.0f));
		mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(3.0, 3.0f, 3.0f));

		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(0);

		// Textura TIERRA
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
		glBindTexture(GL_TEXTURE_2D, earthNormalMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, earthTexture);

		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		
		glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	}
	mvStack.pop();

	//////////// LUNA
	mvStack.push(mvStack.top());
	{
		// traslada y rota
		mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * radius, 0.0f, cos((float)currentTime) * radius));
		mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime * 2, glm::vec3(0.0, 1.0, 0.0));
		mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));

		invTrMat = glm::transpose(glm::inverse(mvStack.top()));
		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
		glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(0);
		// Textura TIERRA
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		// Textura LUNA
		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(3);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, moonNormalMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, moonTexture);

		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	}
	mvStack.pop();
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
	GLFWwindow *window = glfwCreateWindow(1200, 850, "Tarea PC4", NULL, NULL);
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