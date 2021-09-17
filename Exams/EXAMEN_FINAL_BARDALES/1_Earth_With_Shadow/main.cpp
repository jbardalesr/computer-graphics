#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Utils.h"
#include "Sphere.h"
#define numVAOs 1
#define numVBOs 5

using namespace std;

void passOne(double);
void passTwo(double);

float toRadians(float degrees) { return (degrees * 3.14159f) / 180.0f; }

GLuint renderingProgram1, renderingProgram2;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

glm::vec3 earthLoc(0.0f, 0.0f, 0.0f);
glm::vec3 cameraLoc(0.0f, 6.0f, 12.0f);
glm::vec3 lightLoc(3.0f, 3.0f, 25.0f);

// white light
float globalAmbient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
float lightAmbient[4] = {0.0f, 0.0f, 0.0f, 1.0f};
float lightDiffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float lightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};

// gold material
float *gMatAmb = Utils::goldAmbient();
float *gMatDif = Utils::goldDiffuse();
float *gMatSpe = Utils::goldSpecular();
float gMatShi = Utils::goldShininess();

// shadow stuff
int scSizeX, scSizeY;
GLuint shadowTex, shadowBuffer;
glm::mat4 lightVmatrix;
glm::mat4 lightPmatrix;
glm::mat4 shadowMVP1;
glm::mat4 shadowMVP2;
glm::mat4 b;
float radius = 6.0f;

// variable allocation for display
GLuint mvLoc, projLoc, nLoc, sLoc;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, matAmbLoc, matDiffLoc, matSpecLoc, matShiLoc;
float lightPos[3];

GLuint earthTexture, moonTexture;
GLuint moonNormalMap, earthNormalMap;

// MVP matrix
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat;
glm::vec3 currentLightPos, transformed;

// lookAt Camera
glm::vec3 origin(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);

int width, height;
float aspect;

Sphere mySphere = Sphere(48);

// index
enum indexObject
{
	SPHERE_POSITION = 0,
	SPHERE_TEXTURE = 1,
	SPHERE_NORMAL = 2,
	SPHERE_TANGENT = 3
};

void installLights(int renderingProgram, glm::mat4 vMatrix)
{
	transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	// get the locations of the light and material fields in the shader
	globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
	ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
	diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
	specLoc = glGetUniformLocation(renderingProgram, "light.specular");
	posLoc = glGetUniformLocation(renderingProgram, "light.position");
	matAmbLoc = glGetUniformLocation(renderingProgram, "material.ambient");
	matDiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
	matSpecLoc = glGetUniformLocation(renderingProgram, "material.specular");
	matShiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
	glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);
	glProgramUniform4fv(renderingProgram, matAmbLoc, 1, gMatAmb);
	glProgramUniform4fv(renderingProgram, matDiffLoc, 1, gMatDif);
	glProgramUniform4fv(renderingProgram, matSpecLoc, 1, gMatSpe);
	glProgramUniform1f(renderingProgram, matShiLoc, gMatShi);
}

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

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_TEXTURE]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_TANGENT]);
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

void init(GLFWwindow *window)
{
	renderingProgram1 = Utils::createShaderProgram("./vertShadowShader.glsl", "./fragShadowShader.glsl");
	renderingProgram2 = Utils::createShaderProgram("./vertPhongShader.glsl", "./fragPhongShader.glsl");

	earthTexture = Utils::loadTexture("img/earth.jpg");
	earthNormalMap = Utils::loadTexture("img/earthMAP.png");
	moonTexture = Utils::loadTexture("img/moon.jpg");
	moonNormalMap = Utils::loadTexture("img/moonNORMAL.jpg");

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

// ==========================================================
void display(GLFWwindow *window, double currentTime)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	currentLightPos = lightLoc;

	lightVmatrix = glm::lookAt(currentLightPos, origin, up);
	lightPmatrix = glm::perspective(toRadians(60.0f), aspect, 0.1f, 1000.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTex, 0);

	glDrawBuffer(GL_NONE);
	glEnable(GL_DEPTH_TEST);
	// para evitar el acne de sombra
	glEnable(GL_POLYGON_OFFSET_FILL); // for reducing
	glPolygonOffset(2.0f, 4.0f);	  //  shadow artifacts

	passOne(currentTime);

	// para evitar el acne de sombra
	glDisable(GL_POLYGON_OFFSET_FILL); // artifact reduction, continued

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	glDrawBuffer(GL_FRONT);

	passTwo(currentTime);
}

// ==========================================================
void passOne(double currentTime)
{
	glUseProgram(renderingProgram1);

	// draw the EARTH
	mMat = glm::translate(glm::mat4(1.0f), earthLoc);
	mMat = glm::scale(mMat, glm::vec3(3.0, 3.0f, 3.0f));

	shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
	sLoc = glGetUniformLocation(renderingProgram1, "shadowMVP");
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_POSITION]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());

	// draw the MOON
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * radius, 0.0f, cos((float)currentTime) * radius));
	mMat = glm::scale(mMat, glm::vec3(1.5f, 1.5f, 1.5f));

	shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_POSITION]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
}
void passTwo(double currentTime)
{
	glUseProgram(renderingProgram2);

	vMat = glm::lookAt(glm::vec3(cameraLoc.x, cameraLoc.y, cameraLoc.z), // ubicado en este lugar siguiendo una trayectoria circular
					   glm::vec3(0.0f, 0.0f, 0.0f),						 // mira al cero
					   glm::vec3(0.0f, 1.0f, 0.0f));					 // definimos dos variables uniformes

	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram2, "norm_matrix");
	sLoc = glGetUniformLocation(renderingProgram2, "shadowMVP");

	mMat = glm::translate(glm::mat4(1.0f), earthLoc);
 	mMat = rotate(mMat, (float)currentTime / 3, glm::vec3(0.0, 1.0, 0.0));
	mMat = glm::scale(mMat, glm::vec3(3.0, 3.0f, 3.0f));

	currentLightPos = lightLoc;
	installLights(renderingProgram2, vMat);

	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));
	shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

	// draw the EARTH
	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_POSITION]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_TEXTURE]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_NORMAL]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_TANGENT]);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthNormalMap);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthTexture);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());

	// draw the MOON
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * radius, 0.0f, cos((float)currentTime) * radius));
	mMat = glm::scale(mMat, glm::vec3(1.5f, 1.5f, 1.5f));
	mMat = rotate(mMat, (float)currentTime, glm::vec3(0.0, 1.0, 0.0));

	currentLightPos = lightLoc;
	installLights(renderingProgram2, vMat);

	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));
	shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_NORMAL]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_TEXTURE]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_NORMAL]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SPHERE_TANGENT]);
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
	GLFWwindow *window = glfwCreateWindow(1200, 800, "Chapter8 - program2", NULL, NULL);
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