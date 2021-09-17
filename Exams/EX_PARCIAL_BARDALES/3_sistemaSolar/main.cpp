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

#define numVAOs 1
#define numVBOs 3

float cameraX, cameraY, cameraZ;

GLuint sunTexture;
GLuint earthTexture;
GLuint moonTexture;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// variable allocation for display
GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;

// pila de matrices 4x4
stack<glm::mat4> mvStack;
Sphere mySphere = Sphere(48);

void setupVertices(void)
{
	std::vector<int> ind = mySphere.getIndices();
	// no confundir indices de la esfera con los vertices de la esfera, ver la esfera con triangulos
	std::vector<glm::vec3> vert = mySphere.getVertices();
	std::vector<glm::vec2> tex = mySphere.getTexCoords();
	std::vector<glm::vec3> norm = mySphere.getNormals();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	int numIndices = mySphere.getNumIndices();
	for (int i = 0; i < numIndices; i++)
	{
		pvalues.push_back((vert[ind[i]]).x);
		pvalues.push_back((vert[ind[i]]).y);
		pvalues.push_back((vert[ind[i]]).z);
		// hacemos lo mismo con las texturas
		tvalues.push_back((tex[ind[i]]).s);
		tvalues.push_back((tex[ind[i]]).t);
		// las normales
		nvalues.push_back((norm[ind[i]]).x);
		nvalues.push_back((norm[ind[i]]).y);
		nvalues.push_back((norm[ind[i]]).z);
	}

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
}

void init(GLFWwindow *window)
{
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	cameraX = 0.0f;
	cameraY = 5.0f;
	cameraZ = 18.0f;

	setupVertices();

	sunTexture = Utils::loadTexture("img/sun.jpg");
	earthTexture = Utils::loadTexture("img/earth.jpg");
	moonTexture = Utils::loadTexture("img/moon.jpg");
}

void display(GLFWwindow *window, double currentTime)
{
	// radio eliptico
	float a = 2.5f;
	float b = 1.0f;

	// limpiamos el buffer
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	// definimos dos variables uniformes
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	// Matriz transfoma las coordenadas mundo a cooordenas cámara.
	vMat = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), // ubicado en este lugar siguiendo una trayectoria circular
					   glm::vec3(0.0f, 0.0f, 0.0f),			 // mira al cero
					   glm::vec3(0.0f, 1.0f, 0.0f));
	mvStack.push(vMat);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	//////////// SOL
	mvStack.push(mvStack.top());
	// la cima le multiplicamos por la matrix de traslación
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	mvStack.push(mvStack.top());
	// lo que tengas arriba mutiplica por una matriz de rotación y dejalo ahí.
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0, 0.0, 0.0));
	{
		mvStack.push(mvStack.top());
		{
			mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(-3.0, 0.0f, 0.0f));
			mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(2.5, 2.5f, 2.5f));

			glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
			glEnableVertexAttribArray(0);

			// Textura Sol
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, sunTexture);

			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());

			// Graficado del Sol
		}
		mvStack.pop();
	}
	// una vez grafica la pirada eliminamos lo que tiene en la cima de la pila.
	mvStack.pop();

	//////////// TIERRA
	mvStack.push(mvStack.top());

	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(a * sin((float)currentTime) * 4.0, 0.0f, b * cos((float)currentTime) * 4.0));
	mvStack.push(mvStack.top());

	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
	{
		mvStack.push(mvStack.top());
		{
			mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 0.0f));
			glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
			glEnableVertexAttribArray(0);

			// Textura Tierra
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, earthTexture);

			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			// Graficado de la Tierra
			glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
		}
		mvStack.pop();
	}
	mvStack.pop();

	//////////// LUNA
	mvStack.push(mvStack.top());
	{
		// radio eliptico
		a = 1.8f;
		b = 1.5f;

		mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(a * sin((float)currentTime) * 2.0, b * cos((float)currentTime) * 2.0, 0.0f));
		mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 0.0, 1.0));
		mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(0.6, 0.6f, 0.6f));

		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(0);

		// Textura Luna
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, moonTexture);

		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		// Graficado de la Luna
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
	GLFWwindow *window = glfwCreateWindow(900, 600, "Chapter 4 - program 4", NULL, NULL);
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