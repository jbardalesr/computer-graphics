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
#include "ImportedModel.h"
#include <stack>
using namespace std;

#define numVAOs 1
#define numVBOs 10
# define NUMBER_TREE 10

glm::vec3 camera(0.0f, 5.0f, 6.0f);
stack<glm::mat4> mStack;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

GLuint mvLoc, projLoc, colorObj, isColor;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;
float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

GLuint tableroTexture, arbolTexture, carroTexture;
//ImportedModel carroModel("carro.obj");
ImportedModel arbolModel("tree.obj");
ImportedModel carroModel("carr2.obj");

float mover = 0.0f;

enum indexObject
{
	PLANE_POSITION = 0,
	PLANE_TEXTURE = 1,
	CARRO_POSITION = 2,
	CARRO_TEXTURE = 3,
	CARRO_NORMAL = 4,
	ARBOL_POSITION = 5,
	ARBOL_TEXTURE = 6,
	ARBOL_NORMAL = 7
};

glm::vec3 treePosition[NUMBER_TREE] = {glm::vec3(-8.0f, 0.0f, 2.0f),
							  glm::vec3(8.0f, 0.0f, 2.0f),
							  glm::vec3(8.0f, 0.0f, -6.0f),
							  glm::vec3(-8.0f, 0.0f, -6.0f),
							  glm::vec3(8.0f, 0.0f, -14.0f),
							  glm::vec3(-8.0f, 0.0f, -14.0f),
							  glm::vec3(8.0f, 0.0f, -22.0f),
							  glm::vec3(-8.0f, 0.0f, -22.0f),
							  glm::vec3(8.0f, 0.0f, -30.0f),
							  glm::vec3(-8.0f, 0.0f, -30.0f)};

void setupPlano(void)
{
	// wall
	float wallPosition[18] = {-5.0, 0.0, -50.0,
							  5.0, 0.0, 10.0,
							  -5.0, 0.0, 10.0,
							  -5.0, 0.0, -50.0,
							  5.0, 0.0, 10.0,
							  5.0, 0.0, -50.0};

	float textureCoordinates[12] = {0.0, 5.0,
									1.0, 0.0,
									0.0, 0.0,
									0.0, 5.0,
									1.0, 0.0,
									1.0, 5.0};

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[PLANE_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallPosition), &wallPosition[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[PLANE_TEXTURE]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), &textureCoordinates[0], GL_STATIC_DRAW);
}

void setupCarro()
{
	std::vector<glm::vec3> vert = carroModel.getVertices();
	std::vector<glm::vec2> tex = carroModel.getTextureCoords();
	std::vector<glm::vec3> norm = carroModel.getNormals();
	std::vector<float> ptvalues;
	std::vector<float> ttvalues;
	std::vector<float> ntvalues;

	for (int i = 0; i < carroModel.getNumVertices(); i++)
	{
		ptvalues.push_back((vert[i]).x);
		ptvalues.push_back((vert[i]).y);
		ptvalues.push_back((vert[i]).z);

		ttvalues.push_back((tex[i]).s);
		ttvalues.push_back((tex[i]).t);

		ntvalues.push_back((norm[i]).x);
		ntvalues.push_back((norm[i]).y);
		ntvalues.push_back((norm[i]).z);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo[CARRO_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, ptvalues.size() * 4, &ptvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[CARRO_TEXTURE]);
	glBufferData(GL_ARRAY_BUFFER, ttvalues.size() * 4, &ttvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[CARRO_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, ntvalues.size() * 4, &ntvalues[0], GL_STATIC_DRAW);
}

void setupArbol()
{
	std::vector<glm::vec3> vert = arbolModel.getVertices();
	std::vector<glm::vec2> tex = arbolModel.getTextureCoords();
	std::vector<glm::vec3> norm = arbolModel.getNormals();
	std::vector<float> ptvalues;
	std::vector<float> ttvalues;
	std::vector<float> ntvalues;

	for (int i = 0; i < arbolModel.getNumVertices(); i++)
	{
		ptvalues.push_back((vert[i]).x);
		ptvalues.push_back((vert[i]).y);
		ptvalues.push_back((vert[i]).z);

		ttvalues.push_back((tex[i]).s);
		ttvalues.push_back((tex[i]).t);

		ntvalues.push_back((norm[i]).x);
		ntvalues.push_back((norm[i]).y);
		ntvalues.push_back((norm[i]).z);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo[ARBOL_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, ptvalues.size() * 4, &ptvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[ARBOL_TEXTURE]);
	glBufferData(GL_ARRAY_BUFFER, ttvalues.size() * 4, &ttvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[ARBOL_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, ntvalues.size() * 4, &ntvalues[0], GL_STATIC_DRAW);
}

void init(GLFWwindow *window)
{
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	setupPlano();
	setupCarro();
	setupArbol();

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	tableroTexture = Utils::loadTexture("pista.jpg");
	arbolTexture = Utils::loadTexture("tree_texture.jpg");
}

void display(GLFWwindow *window, double currentTime)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	colorObj = glGetUniformLocation(renderingProgram, "color_obj");
	isColor = glGetUniformLocation(renderingProgram, "is_color");

	vMat = glm::lookAt(camera,
					   glm::vec3(0.0f, 0.0f, 0.0f),
					   glm::vec3(0.0f, 1.0f, 0.0f));

	// DRAW CARRO
	glUniform1f(isColor, 1.0f);
	mMat = glm::translate(glm::mat4(1.0f), treePosition[0]);
	mMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	mMat = glm::rotate(mMat, toRadians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	mvMat = vMat * mMat;

	glUniform4f(colorObj, 0.75294f, 0.75294f, 0.75294f, 1.0f);
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[CARRO_POSITION]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// TEXTURA
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, carroModel.getNumVertices());

	//////////////////
	// DRAW PLANE
	if (mover > 8.0){
		mover = 0.0;
	} 
	mover += 0.2;
	
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, mover));
	mStack.push(mMat);
	{
		mStack.push(mStack.top());
		{
			mStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));

			mvMat = vMat * mStack.top();

			glUniform1f(isColor, 0.0f);
			glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

			glBindBuffer(GL_ARRAY_BUFFER, vbo[PLANE_POSITION]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo[PLANE_TEXTURE]);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tableroTexture);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
		}
		mStack.pop();
		mStack.push(mStack.top());
		{
			// DRAW TREE
			for (int i = 0; i < NUMBER_TREE; i++)
			{
				mMat = glm::translate(mStack.top(), treePosition[i]);
				mMat = glm::scale(mMat, glm::vec3(0.1f, 0.1f, 0.1f));

				mvMat = vMat * mMat;

				glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

				glBindBuffer(GL_ARRAY_BUFFER, vbo[ARBOL_POSITION]);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				// TEXTURA
				glBindBuffer(GL_ARRAY_BUFFER, vbo[ARBOL_TEXTURE]);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(1);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, arbolTexture);

				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
				glDrawArrays(GL_TRIANGLES, 0, arbolModel.getNumVertices());
			}
		}
	}
	mStack.pop();
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
	GLFWwindow *window = glfwCreateWindow(1000, 600, "Chapter5 - program1", NULL, NULL);
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