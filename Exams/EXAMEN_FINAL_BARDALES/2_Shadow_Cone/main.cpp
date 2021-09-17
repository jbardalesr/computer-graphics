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
#include "Cone.h"
#include "ImportedModel.h"
#include "Torus.h"

using namespace std;

void passOne(void);
void passTwo(void);

#define numVAOs 1
#define numVBOs 10

float aspect;

GLuint vLoc, projLoc;
GLuint renderingProgram1, renderingProgram2;
GLuint vao[numVAOs], vbo[numVBOs];
GLuint mvLoc, nLoc, colorObj, isGolden;

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
float globalAmbient[4] = {0.2f, 0.2f, 0.2f, 1.0f};

float spotLightAmbient[4] = {0.0f, 0.0f, 0.0f, 1.0f};
float spotLightDiffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float spotLightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
glm::vec3 spotLightLocation = glm::vec3(-6.0f, 6.0f, 2.0f);
float spotLightDirection[3] = {-5.0f, 3.0f, 4.0f};
float spotLightCutoff = glm::cos(toRadians(22.0f));
float spotLightExponent = 10.0f;
void installLights(glm::mat4 vMatrix);

// gold material properties
float *matAmb = Utils::goldAmbient();
float *matDif = Utils::goldDiffuse();
float *matSpe = Utils::goldSpecular();
float matShi = Utils::goldShininess();

glm::vec3 planeLoc(0.0f, 0.0f, 0.0f);
glm::vec3 coneLoc(0.0f, 0.0f, 0.0f);
glm::vec3 torusLoc(3.0f, 0.0f, 0.0f);
glm::vec3 cameraLoc(0.0f, 3.0f, 6.0f);

stack<glm::mat4> mStack;
Torus myTorus(0.6f, 0.4f, 48);

enum indexObject
{
	PLANE_POSITION = 0,
	PLANE_NORMAL = 1,
	CONE_POSITION = 2,
	CONE_NORMALS = 3,
	TORUS_POSITION = 4,
	TORUS_NORMALS = 5,
};

ImportedModel coneObj("Cone.obj");
ImportedModel torusObj("Torus.obj");
ImportedModel planeObj("wall.obj");

// shadow stuff
int scSizeX, scSizeY;
GLuint shadowTex, shadowBuffer;
glm::mat4 lightVmatrix;
glm::mat4 lightPmatrix;
glm::mat4 shadowMVP1;
glm::mat4 shadowMVP2;
glm::mat4 b;
int numConeVertices, numTorusVertices, numPlaneVertices;

void setupVertices(void)
{
	// Cone definition
	numPlaneVertices = planeObj.getNumVertices();
	std::vector<glm::vec3> vertPlane = planeObj.getVertices();
	std::vector<glm::vec3> normPlane = planeObj.getNormals();

	std::vector<float> planePosition;
	std::vector<float> planeNormal;

	for (int i = 0; i < numPlaneVertices; i++)
	{
		planePosition.push_back((vertPlane[i]).x);
		planePosition.push_back((vertPlane[i]).y);
		planePosition.push_back((vertPlane[i]).z);
		planeNormal.push_back((normPlane[i]).x);
		planeNormal.push_back((normPlane[i]).y);
		planeNormal.push_back((normPlane[i]).z);
	}

	// Cone definition
	numConeVertices = coneObj.getNumVertices();
	std::vector<glm::vec3> vert = coneObj.getVertices();
	std::vector<glm::vec3> norm = coneObj.getNormals();

	std::vector<float> conePosition;
	std::vector<float> coneNormal;

	for (int i = 0; i < numConeVertices; i++)
	{
		conePosition.push_back((vert[i]).x);
		conePosition.push_back((vert[i]).y);
		conePosition.push_back((vert[i]).z);
		coneNormal.push_back((norm[i]).x);
		coneNormal.push_back((norm[i]).y);
		coneNormal.push_back((norm[i]).z);
	}

	// Torus definition
	numTorusVertices = torusObj.getNumVertices();
	std::vector<glm::vec3> vertTorus = torusObj.getVertices();
	std::vector<glm::vec3> normTorus = torusObj.getNormals();

	std::vector<float> torusPosition;
	std::vector<float> torusNormal;

	for (int i = 0; i < numTorusVertices; i++)
	{
		torusPosition.push_back((vertTorus[i]).x);
		torusPosition.push_back((vertTorus[i]).y);
		torusPosition.push_back((vertTorus[i]).z);
		torusNormal.push_back((normTorus[i]).x);
		torusNormal.push_back((normTorus[i]).y);
		torusNormal.push_back((normTorus[i]).z);
	}

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[PLANE_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, planePosition.size() * 4, &planePosition[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[PLANE_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, planeNormal.size() * 4, &planeNormal[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[CONE_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, conePosition.size() * 4, &conePosition[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[CONE_NORMALS]);
	glBufferData(GL_ARRAY_BUFFER, coneNormal.size() * 4, &coneNormal[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TORUS_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, torusPosition.size() * 4, &torusPosition[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TORUS_NORMALS]);
	glBufferData(GL_ARRAY_BUFFER, torusNormal.size() * 4, &torusNormal[0], GL_STATIC_DRAW);
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
 	mMat = rotate(mMat, toRadians(180.0f), glm::vec3(0.0, 1.0, 0.0));

	shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
	sLoc = glGetUniformLocation(renderingProgram1, "shadowMVP");
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[PLANE_POSITION]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glClear(GL_DEPTH_BUFFER_BIT);
	// glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, numPlaneVertices);

	// draw the CONE
	mMat = glm::translate(glm::mat4(1.0f), coneLoc);
	shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[CONE_POSITION]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glClear(GL_DEPTH_BUFFER_BIT);
	// glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, numConeVertices);

	// draw the TORUS
	mMat = glm::translate(glm::mat4(1.0f), torusLoc);
	shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TORUS_POSITION]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//glClear(GL_DEPTH_BUFFER_BIT);
	// glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, numTorusVertices);
}

void passTwo(void)
{
	glUseProgram(renderingProgram2);

	vMat = glm::lookAt(glm::vec3(cameraLoc.x, cameraLoc.y, cameraLoc.z),
					   glm::vec3(0.0f, 0.0f, 0.0f),
					   glm::vec3(0.0f, 1.0f, 0.0f));

	// DRAW SCENE
	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram2, "norm_matrix");
	colorObj = glGetUniformLocation(renderingProgram2, "color_obj");
	isGolden = glGetUniformLocation(renderingProgram2, "is_golden");
	sLoc = glGetUniformLocation(renderingProgram2, "shadowMVP");

	mMat = glm::translate(glm::mat4(1.0f), planeLoc);

	//////////////////////////////////////////////////////////////////
	mStack.push(mMat);
	{
		// PLANE
		mStack.push(mStack.top());
		{
			installLights(vMat);
			mStack.top() *= glm::translate(glm::mat4(1.0f), planeLoc);
 			mStack.top() *= rotate(glm::mat4(1.0f), toRadians(180.0f), glm::vec3(0.0, 1.0, 0.0));

			mvMat = vMat * mStack.top();
			invTrMat = glm::transpose(glm::inverse(mvMat));
			shadowMVP2 = b * lightPmatrix * lightVmatrix * mStack.top();

			glUniform1f(isGolden, 0.0f);
			glUniform4f(colorObj, 0.0f, 1.0f, 0.0f, 1.0f);
			glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
			glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
			glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

			glBindBuffer(GL_ARRAY_BUFFER, vbo[PLANE_POSITION]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);

			// contiene los normales
			glBindBuffer(GL_ARRAY_BUFFER, vbo[PLANE_NORMAL]);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);

			glClear(GL_DEPTH_BUFFER_BIT);
			// glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDrawArrays(GL_TRIANGLES, 0, numPlaneVertices);
		}
		mStack.pop();

		// CONE
		mStack.push(mStack.top());
		{
			installLights(vMat);
			mStack.top() *= glm::translate(glm::mat4(1.0f), coneLoc);

			mvMat = vMat * mStack.top();

			invTrMat = glm::transpose(glm::inverse(mvMat));
			shadowMVP2 = b * lightPmatrix * lightVmatrix * mStack.top();

			glUniform4f(colorObj, 0.450980f, 0.0f, 0.019608f, 1.0f);
			glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
			glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
			glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

			glBindBuffer(GL_ARRAY_BUFFER, vbo[CONE_POSITION]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);

			// contiene los normales
			glBindBuffer(GL_ARRAY_BUFFER, vbo[CONE_NORMALS]);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);

			// glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDrawArrays(GL_TRIANGLES, 0, numConeVertices);
		}
		mStack.pop();
		// TORUS
		mStack.push(mStack.top());
		{
			installLights(vMat);
			mStack.top() *= glm::translate(glm::mat4(1.0f), torusLoc);

			mvMat = vMat * mStack.top();

			invTrMat = glm::transpose(glm::inverse(mvMat));
			shadowMVP2 = b * lightPmatrix * lightVmatrix * mStack.top();

			glUniform1f(isGolden, 1.0f);
			glUniform4f(colorObj, 1.0f, 0.0f, 0.0f, 1.0f);
			glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
			glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
			glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

			glBindBuffer(GL_ARRAY_BUFFER, vbo[TORUS_POSITION]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);

			// contiene los normales
			glBindBuffer(GL_ARRAY_BUFFER, vbo[TORUS_NORMALS]);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);

			// glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDrawArrays(GL_TRIANGLES, 0, numTorusVertices);
		}
		mStack.pop();
	}
	mStack.pop();
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
	GLFWwindow *window = glfwCreateWindow(1000, 640, "Sustitutorio Cone Shadow", NULL, NULL);
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
