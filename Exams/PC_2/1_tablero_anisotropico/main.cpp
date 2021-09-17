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
using namespace std;

/* Que compile todo el cpp para el ejecutable
g++ -Wall *.cpp -lmysqlclient -lm -lopencv_core -lopencv_highgui -lopencv_imgcodecs `pkg-config --libs --cflags opencv4` -lssl -lcrypto -lGL -lGLU -lglut -lGLEW -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -I/usr/local/include/opencv4/ 
*/

// instalacion de OpenCV
// https://gist.github.com/bikz05/a8aa07adf89cd5fb89fb
#define numVAOs 1
#define numVBOs 2

float cameraX, cameraY, cameraZ;
float pyrLocX, pyrLocY, pyrLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// variable allocation for display
GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;

GLuint tableroTexture;

void setupVertices(void)
{
	float pyramidPositions[18] = {
		-1.0, 1.0, -4.0,
		1.0, -1.0, 5.0,
		-1.0, -1.0, 5.0,
		-1.0, 1.0, -4.0,
		1.0, 1.0, -4.0, 
		1.0, -1.0, 5.0 
	};
	// coordenadas en 2D asociado a cada en 2D de la textura.
	// texel pixel correspondiente de una textura.
	float textureCoordinates[12] = {
		0.0, 1.0,
		1.0, 0.0,
		0.0, 0.0, 
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0
	};

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	// dos vbos que almacenen las cooredenas de la piramide y la textura.
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), &pyramidPositions[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), &textureCoordinates[0], GL_STATIC_DRAW);
}

void init(GLFWwindow *window)
{
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	cameraX = 0.0f;
	cameraY = 0.5f;
	cameraZ = 5.0f;
	pyrLocX = 0.0f;
	pyrLocY = 0.0f;
	pyrLocZ = 0.0f;
	setupVertices();

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	tableroTexture = Utils::loadTexture("chess-board.png");
}

void display(GLFWwindow *window, double currentTime)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(pyrLocX, pyrLocY, pyrLocZ));

	mMat = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));

	mvMat = vMat * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// tenemos que enviarle a nuestro shader este ID de textura
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, tableroTexture);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, 18);
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
	GLFWwindow *window = glfwCreateWindow(600, 600, "Chapter5 - program1", NULL, NULL);
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