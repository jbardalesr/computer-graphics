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
#include "ImportedModel.h"

using namespace std;

#define numVAOs 1
#define numVBOs 20

float cameraX, cameraY, cameraZ;

float aspect;

GLuint vLoc, projLoc;
GLuint renderingProgram, renderingProgramCubeMap, renderingProgramPlane;
GLuint vao[numVAOs], vbo[numVBOs];
GLuint mvpLoc, mvLoc, color_obj, object, nLoc;

int width, height;

GLuint planeTexture, fullBoxTexture, fullTNTBoxTexture, skyBoxTexture, planeImage, planeHeight, planeNormal, treeTexture;

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

// LUZ
// locations for shader uniform variables
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mAmbLoc, mDiffLoc, mSpecLoc, mShiLoc;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat, view, mvpMat;

// light position as Vector3f, in both model and view space
glm::vec3 currentLightPos, lightPosV;

// light position as float array
float lightPos[3];

// initial light location
glm::vec3 initialLightLoc = glm::vec3(5.0f, 2.0f, 2.0f);
// white light properties
float globalAmbient[4] = {0.8f, 0.8f, 0.8f, 1.0f};
float lightAmbient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
float lightDiffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float lightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};

glm::vec3 boxTNTPosition[4] = {glm::vec3(20.0f, 4.0f, -10.0f),
							   glm::vec3(28.0f, 4.0f, -10.0f),
							   glm::vec3(28.0f, 4.0f, -18.0f),
							   glm::vec3(20.0f, 4.0f, -18.0f)};

glm::vec3 boxPosition[12] = {glm::vec3(24.0f, 10.0f, -14.0f),
							 glm::vec3(0.0, 4.0f, 16.0f),
							 glm::vec3(8.0f, 4.0f, 16.0f),
							 glm::vec3(16.0f, 4.0f, 16.0f),
							 glm::vec3(0.0f, 4.0f, 8.0f),
							 glm::vec3(8.0f, 4.0f, 8.0f),
							 glm::vec3(16.0f, 4.0f, 8.0f),
							 glm::vec3(0.0f, 4.0f, 16.0f),
							 glm::vec3(8.0f, 4.0f, 16.0f),
							 glm::vec3(16.0f, 4.0f, 16.0f),
							 glm::vec3(12.0f, 10.0f, 12.0f),
							 glm::vec3(4.0f, 10.0f, 12.0f)};

glm::vec3 treePosition[4] = {glm::vec3(-50.0f, 0.0f, 50.0f),
							 glm::vec3(50.0f, 0.0f, 50.0f),
							 glm::vec3(50.0f, 0.0f, -50.0f),
							 glm::vec3(-50.0f, 0.0f, -50.0f)};

// silver material properties
float *matAmb = Utils::silverAmbient();
float *matDif = Utils::silverDiffuse();
float *matSpe = Utils::silverSpecular();
float matShi = Utils::silverShininess();

// gold material properties
float *matAmbBronze = Utils::bronzeAmbient();
float *matDifBronze = Utils::bronzeDiffuse();
float *matSpeBronze = Utils::bronzeSpecular();
float matShiBronze = Utils::bronzeShininess();

stack<glm::mat4> mvStack;

ImportedModel tetsujinModel("models/tetsujin/tetsujin_fight.obj");
ImportedModel fullBoxModel("models/box/full_box.obj");
ImportedModel treeModel("models/tree.obj");
ImportedModel dogModel("models/dog.obj");

// Sphere mySphere = Sphere(48);

float lightMovement = 0.01f;
float lightLocX, lightLocY, lightLocZ;

enum indexObject
{
	PLANE_POSITION = 0,
	PLANE_TEXTURE = 1,
	BOX_POSITION = 2,
	BOX_TEXTURE = 3,
	BOX_NORMAL = 4,
	TETSUJIN_POSITION = 5,
	TETSUJIN_TEXTURE = 6,
	TETSUJIN_NORMAL = 7,
	SKY_POSITION = 8,
	SKY_TEXTURE = 9,
	SKY_NORMAL = 10,
	TREE_POSITION = 11,
	TREE_TEXTURE = 12,
	TREE_NORMAL = 13,
	DOG_POSITION = 14,
	DOG_TEXTURE = 15,
	DOG_NORMAL = 16,
};

void setupTetsujin()
{
	std::vector<glm::vec3> vert = tetsujinModel.getVertices();
	std::vector<glm::vec2> tex = tetsujinModel.getTextureCoords();
	std::vector<glm::vec3> norm = tetsujinModel.getNormals();
	std::vector<float> ptvalues;
	std::vector<float> ttvalues;
	std::vector<float> ntvalues;

	for (int i = 0; i < tetsujinModel.getNumVertices(); i++)
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

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TETSUJIN_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, ptvalues.size() * 4, &ptvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TETSUJIN_TEXTURE]);
	glBufferData(GL_ARRAY_BUFFER, ttvalues.size() * 4, &ttvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TETSUJIN_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, ntvalues.size() * 4, &ntvalues[0], GL_STATIC_DRAW);
}

void setupTree()
{
	std::vector<glm::vec3> vert = treeModel.getVertices();
	std::vector<glm::vec2> tex = treeModel.getTextureCoords();
	std::vector<glm::vec3> norm = treeModel.getNormals();
	std::vector<float> ptvalues;
	std::vector<float> ttvalues;
	std::vector<float> ntvalues;

	for (int i = 0; i < treeModel.getNumVertices(); i++)
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

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TREE_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, ptvalues.size() * 4, &ptvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TREE_TEXTURE]);
	glBufferData(GL_ARRAY_BUFFER, ttvalues.size() * 4, &ttvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TREE_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, ntvalues.size() * 4, &ntvalues[0], GL_STATIC_DRAW);
}

void setupDog()
{
	std::vector<glm::vec3> vert = dogModel.getVertices();
	std::vector<glm::vec2> tex = dogModel.getTextureCoords();
	std::vector<glm::vec3> norm = dogModel.getNormals();
	std::vector<float> ptvalues;
	std::vector<float> ttvalues;
	std::vector<float> ntvalues;

	for (int i = 0; i < dogModel.getNumVertices(); i++)
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

	glBindBuffer(GL_ARRAY_BUFFER, vbo[DOG_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, ptvalues.size() * 4, &ptvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[DOG_TEXTURE]);
	glBufferData(GL_ARRAY_BUFFER, ttvalues.size() * 4, &ttvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[DOG_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, ntvalues.size() * 4, &ntvalues[0], GL_STATIC_DRAW);
}

void setupBox()
{
	std::vector<glm::vec3> vert = fullBoxModel.getVertices();
	std::vector<glm::vec2> tex = fullBoxModel.getTextureCoords();
	std::vector<glm::vec3> norm = fullBoxModel.getNormals();
	std::vector<float> ptvalues;
	std::vector<float> ttvalues;
	std::vector<float> ntvalues;

	for (int i = 0; i < fullBoxModel.getNumVertices(); i++)
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

	glBindBuffer(GL_ARRAY_BUFFER, vbo[BOX_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, ptvalues.size() * 4, &ptvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[BOX_TEXTURE]);
	glBufferData(GL_ARRAY_BUFFER, ttvalues.size() * 4, &ttvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[BOX_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, ntvalues.size() * 4, &ntvalues[0], GL_STATIC_DRAW);
}

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

	// PLANO
	float planePosition[18] = {-100.0, 0.0, -100.0,
							   100.0, 0.0, 100.0,
							   -100.0, 0.0, 100.0,
							   -100.0, 0.0, -100.0,
							   100.0, 0.0, 100.0,
							   100.0, 0.0, -100.0};

	// coordenadas en 2D asociado a cada en 2D de la textura.
	// texel pixel correspondiente de una textura.
	float planeTextureMap[12] = {0.0, 1.0,
								 1.0, 0.0,
								 0.0, 0.0,
								 0.0, 1.0,
								 1.0, 0.0,
								 1.0, 1.0};

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SKY_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexPositions), cubeVertexPositions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[PLANE_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planePosition), &planePosition[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[PLANE_TEXTURE]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeTextureMap), &planeTextureMap[0], GL_STATIC_DRAW);
}

void installLights(glm::mat4 vMatrix, GLuint renderingProgram, int golden)
{
	// convert light's position to view space, and save it in a float array
	lightPosV = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = lightPosV.x;
	lightPos[1] = lightPosV.y;
	lightPos[2] = lightPosV.z;

	// get the locations of the light and material fields in the shader
	globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
	ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
	diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
	specLoc = glGetUniformLocation(renderingProgram, "light.specular");
	posLoc = glGetUniformLocation(renderingProgram, "light.position");
	mAmbLoc = glGetUniformLocation(renderingProgram, "material.ambient");
	mDiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
	mSpecLoc = glGetUniformLocation(renderingProgram, "material.specular");
	mShiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

	// set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
	glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);

	if (golden == 1)
	{
		glProgramUniform4fv(renderingProgram, mAmbLoc, 1, matAmbBronze);
		glProgramUniform4fv(renderingProgram, mDiffLoc, 1, matDifBronze);
		glProgramUniform4fv(renderingProgram, mSpecLoc, 1, matSpeBronze);
		glProgramUniform1f(renderingProgram, mShiLoc, matShiBronze);
	}
	else
	{
		glProgramUniform4fv(renderingProgram, mAmbLoc, 1, matAmb);
		glProgramUniform4fv(renderingProgram, mDiffLoc, 1, matDif);
		glProgramUniform4fv(renderingProgram, mSpecLoc, 1, matSpe);
		glProgramUniform1f(renderingProgram, mShiLoc, matShi);
	}
}

void init(GLFWwindow *window)
{
	renderingProgram = Utils::createShaderProgram("./PhongShadersHalf/vertShader.glsl", "./PhongShadersHalf/fragShader.glsl");
	renderingProgramPlane = Utils::createShaderProgram("./PlaneTesellationShader/vertShader.glsl", "./PlaneTesellationShader/tessCShader.glsl", "./PlaneTesellationShader/tessEShader.glsl", "./PlaneTesellationShader/fragShader.glsl");
	renderingProgramCubeMap = Utils::createShaderProgram("CubeShaders/vertCubeShader.glsl", "CubeShaders/fragCubeShader.glsl");

	lightLocX = 0.0f;
	lightLocY = 0.01f;
	lightLocZ = 0.2f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices();
	setupTetsujin();
	setupBox();
	setupTree();
	setupDog();

	fullBoxTexture = Utils::loadTexture("img/full_box.jpg");
	skyBoxTexture = Utils::loadCubeMap("img/cubeMap");
	treeTexture = Utils::loadTexture("img/tree_texture.jpg");
	fullTNTBoxTexture = Utils::loadTexture("img/full_box_TNT.jpg");
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	cameraX = 20.0f;
	cameraY = 10.0f;
	cameraZ = 30.0f;

	// textura, imagen y normal mapping
	planeImage = Utils::loadTexture("./img/soil/image.jpg");
	planeHeight = Utils::loadTexture("./img/soil/height.jpg");
	planeNormal = Utils::loadTexture("./img/soil/normal.jpg");
}

void display(GLFWwindow *window, double currentTime, float pos_x, float pos_z, float rot_left, float rot_right, float cam_left, float cam_right)
{
	// llenamos el buffer con (1, 1, 1)
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	const float radius = 50.0f;
	float camX = sin((cam_right + cam_left) / 4) * radius;
	float camZ = cos((cam_right + cam_left) / 4) * radius;

	vMat = glm::lookAt(glm::vec3(camX, cameraY, camZ),
					   glm::vec3(0.0f, 0.0f, 0.0f),
					   glm::vec3(0.0f, 1.0f, 0.0f));

	// DRAW SKY BOX
	glUseProgram(renderingProgramCubeMap);

	vLoc = glGetUniformLocation(renderingProgramCubeMap, "v_matrix");
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));

	projLoc = glGetUniformLocation(renderingProgramCubeMap, "p_matrix");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[SKY_POSITION]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glActiveTexture(GL_TEXTURE0);
	// una textura en 3D
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTexture);

	// glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW); // cube is CW, but we are viewing the inside
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// para el buffer de profundidad lo habilitamos nuevamente
	glEnable(GL_DEPTH_TEST);

	// DRAW PLANE
	glUseProgram(renderingProgramPlane);

	lightLocX += lightMovement;
	if (lightLocX > 0.5)
		lightMovement = -0.01f;
	else if (lightLocX < -0.5)
		lightMovement = 0.01f;

	mvpLoc = glGetUniformLocation(renderingProgramPlane, "mvp_matrix");
	mvLoc = glGetUniformLocation(renderingProgramPlane, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgramPlane, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgramPlane, "norm_matrix");

	mMat = glm::scale(glm::mat4(1.0f), glm::vec3(150.0f, 150.0f, 150.0f));

	mvMat = vMat * mMat;
	mvpMat = pMat * vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	currentLightPos = glm::vec3(lightLocX, lightLocY, lightLocZ);
	installLights(vMat, renderingProgramPlane, 0);

	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	// 3 texturas
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planeImage);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, planeHeight);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, planeNormal);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// un PACTH
	glDrawArraysInstanced(GL_PATCHES, 0, 4, 16 * 16);

	// DRAW SCENE
	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	color_obj = glGetUniformLocation(renderingProgram, "color_obj");
	object = glGetUniformLocation(renderingProgram, "object");
	nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");

	currentLightPos = glm::vec3(initialLightLoc.x, initialLightLoc.y, initialLightLoc.z);
	installLights(vMat, renderingProgram, 0);

	//sceneViewNavigation();

	mvStack.push(vMat);
	{
		//////////////////////////////////////////////////////////////////
		// BOX TNT
		for (int i = 0; i < 4; i++)
		{
			mvStack.push(mvStack.top());
			{
				glUniform1i(object, 1);

				mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(boxTNTPosition[i]));
				mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));

				glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

				glBindBuffer(GL_ARRAY_BUFFER, vbo[BOX_POSITION]);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				// TEXTURA
				glBindBuffer(GL_ARRAY_BUFFER, vbo[BOX_TEXTURE]);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(1);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, fullTNTBoxTexture);
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
				glDrawArrays(GL_TRIANGLES, 0, fullBoxModel.getNumVertices());
			}
			mvStack.pop();
		}
		// BOX NORMAL
		for (int i = 0; i < 12; i++)
		{
			mvStack.push(mvStack.top());
			{
				glUniform1i(object, 1);

				mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(boxPosition[i]));
				mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));

				glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

				glBindBuffer(GL_ARRAY_BUFFER, vbo[BOX_POSITION]);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				// TEXTURA
				glBindBuffer(GL_ARRAY_BUFFER, vbo[BOX_TEXTURE]);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(1);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, fullBoxTexture);
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
				glDrawArrays(GL_TRIANGLES, 0, fullBoxModel.getNumVertices());
			}
			mvStack.pop();
		}
		//////////////////////////////////////////////////////////////////
		// TREEE
		for (int i = 0; i < 4; i++)
		{
			mvStack.push(mvStack.top());
			{
				glUniform1i(object, 1);

				mvStack.top() *= glm::translate(glm::mat4(1.0f), treePosition[i]);
				//mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));

				glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

				glBindBuffer(GL_ARRAY_BUFFER, vbo[TREE_POSITION]);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				// TEXTURA
				glBindBuffer(GL_ARRAY_BUFFER, vbo[TREE_TEXTURE]);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(1);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, treeTexture);

				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
				glDrawArrays(GL_TRIANGLES, 0, treeModel.getNumVertices());
			}
			mvStack.pop();
		}
		//////////////////////////////////////////////////////////////////
		// TETSUJIN
		mvStack.push(mvStack.top());
		{
			glUniform1i(object, 2);

			mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f + pos_x, 1.0f, 0.0f + pos_z));
			mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
			mvStack.top() *= glm::rotate(glm::mat4(1.0f), toRadians(0.0f + rot_left + rot_right), glm::vec3(0.0f, 1.0f, 0.0f));
			invTrMat = glm::transpose(glm::inverse(mvStack.top()));

			glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
			glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

			glBindBuffer(GL_ARRAY_BUFFER, vbo[TETSUJIN_POSITION]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);

			// contiene los normales
			glBindBuffer(GL_ARRAY_BUFFER, vbo[TETSUJIN_NORMAL]);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDrawArrays(GL_TRIANGLES, 0, tetsujinModel.getNumVertices());
		}
		mvStack.pop();
		//////////////////////////////////////////////////////////////////
		// DOG
		mvStack.push(mvStack.top());
		{
			installLights(vMat, renderingProgram, 1);

			glUniform1i(object, 2);

			mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 0.0f));
			mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
			mvStack.top() *= glm::rotate(glm::mat4(1.0f), toRadians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			invTrMat = glm::transpose(glm::inverse(mvStack.top()));

			glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
			glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

			glBindBuffer(GL_ARRAY_BUFFER, vbo[DOG_POSITION]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);

			// contiene los normales
			glBindBuffer(GL_ARRAY_BUFFER, vbo[DOG_NORMAL]);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDrawArrays(GL_TRIANGLES, 0, dogModel.getNumVertices());
		}
		mvStack.pop();
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
	float pos_x = 0.0;
	float pos_z = 0.0;
	float rot_left = 0.0;
	float rot_right = 0.0;
	float cam_left = 0.0;
	float cam_right = 0.0;

	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow *window = glfwCreateWindow(1000, 600, "Test 2 - problem 3", NULL, NULL);

	//glfwSetKeyCallback(window, keyCallback);

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
		display(window, glfwGetTime(), pos_x, pos_z, rot_left, rot_right, cam_left, cam_right);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			cout << "Good Bye world" << endl;
			glfwSetWindowShouldClose(window, true);
		}

		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			pos_x += 0.3;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			pos_x -= 0.3;
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			pos_z += 0.3;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			pos_z -= 0.3;
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			rot_left += 3.0;
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			rot_right -= 3.0;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			cam_left += 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			cam_right -= 0.1;
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
