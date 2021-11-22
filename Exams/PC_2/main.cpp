#include <stdio.h>
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "utils.h"

using namespace std;

/**
 g++ -Wall *.cpp -lmysqlclient -lm -lopencv_core -lopencv_highgui -lopencv_imgcodecs `pkg-config --libs --cflags opencv4` -lssl -lcrypto -lGL -lGLU -lglut -lGLEW -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -I/usr/local/include/opencv4/
 g++ *.cpp -lGL -std=c++11 -Wall -lGL -lGLU -lglut -lGLEW -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor
*/
#define numVAOs 1
#define numVBOs 2

float cameraX, cameraY, cameraZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// variable allocation for display
GLuint mvLoc, projLoc, colorCube;

int width, height;
float aspect;
float timeFactor;
float zrotArm = 0.0f, zrotForeArm = 0.0f, zrotHand = 0.0f;
float incArm = 0.01f, incForeArm = 0.005f, incHand = 0.01f;
glm::mat4 pMat, vMat, mMat, mvMat;
glm::vec4 posArm, foreArm, hand;
stack<glm::mat4> mvStack;

void setupVertices(void)
{
    float cubePosition[108] = {
        -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
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

    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubePosition), cubePosition, GL_STATIC_DRAW);
}

void init(GLFWwindow *window)
{
    renderingProgram = createShaderProgram("vertShader.glsl", "fragShader.glsl");

    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

    // position the camera further down the positive Z axis (to see all of the cubes) lookup
    cameraX = -10.0;
    cameraY = 3.0f;
    cameraZ = 40.0f;
    // left, right, bottom, top
    //pMat = glm::ortho(-(float)width / 32, (float)width / 32, 0.0f, -(float)height / 16);
    // position the camera further down the positive Z axis (to see all of the cubes)
    setupVertices();
}

void display(GLFWwindow *window, double currentTime)
{
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(renderingProgram);

    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    colorCube = glGetUniformLocation(renderingProgram, "color_cube");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
    mvStack.push(vMat);
    {
        // BODY BLUE CUBE ===========================================
        mvStack.push(mvStack.top());
        {
            //mvStack.top() = glm::rotate(mvStack.top(), (float)currentTime / 8.0f, glm::vec3(0, 0.0, 1.0));
            mvStack.push(mvStack.top());
            {
                mvStack.top() = glm::translate(mvStack.top(), glm::vec3(0.0f, 0.0f, -0.1f));
                mvStack.top() = glm::scale(mvStack.top(), glm::vec3(1.0f, 1.0f, 10.0f));

                glUniform4f(colorCube, 0.0f, 0.0f, 1.0f, 1.0f);
                glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

                glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
                glEnableVertexAttribArray(0);
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LEQUAL);
                glDrawArrays(GL_TRIANGLES, 0, 36);

                // ARM GREEN CUBE ==========================================
                mvStack.push(mvStack.top());
                {
                    // posicion original del brazo, lo ponemos detras del cuerpo con -0.1
                    mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, -0.1f));
                    mvStack.push(mvStack.top());
                    {
                        zrotArm += incArm;
                        if (zrotArm > -M_PI / 8.0)
                            incArm = -0.01;
                        else if (zrotArm < -M_PI / 2.0)
                            incArm = 0.01;
                        // proceso de rotacion traslacion escalamiento, ver teoria de vectores
                        mvStack.top() *= glm::rotate(glm::mat4(1.0f), zrotArm, glm::vec3(0.0, 0.0, 1.0));

                        mvStack.push(mvStack.top());
                        {
                            mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f));
                            mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 1.0f, 0.1f));

                            glUniform4f(colorCube, 0.0f, 1.0f, 0.0f, 1.0f);
                            glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

                            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
                            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
                            glEnableVertexAttribArray(0);
                            glDrawArrays(GL_TRIANGLES, 0, 36);
                        }
                        mvStack.pop();

                        // FOREARM LIGHE BLUE CUBE ==========================================
                        foreArm = glm::vec4(7.0f, 0.0f, 0.0f, 1.0); // pos
                        mvStack.push(mvStack.top());
                        {
                            mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(foreArm));
                            mvStack.push(mvStack.top());
                            {
                                zrotForeArm += incForeArm;
                                if (zrotForeArm > 0.0 )
                                    incForeArm = -0.01;
                                else if (zrotForeArm < -M_PI/2.0)
                                    incForeArm = 0.01;

                                mvStack.top() *= glm::rotate(glm::mat4(1.0f), zrotForeArm, glm::vec3(0.0, 0.0, 1.0));
                                mvStack.push(mvStack.top());
                                {
                                    mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
                                    mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 0.1f));

                                    glUniform4f(colorCube, 0.0f, 1.0f, 1.0f, 1.0f);
                                    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

                                    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
                                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
                                    glEnableVertexAttribArray(0);
                                    glDrawArrays(GL_TRIANGLES, 0, 36); 
                                                                
                                }
                                mvStack.pop();

                                //ORANGE CUBE HAND ==========================================
                                hand = glm::vec4(5.0f, 0.0f, 0.0f, 1.0);
                                mvStack.push(mvStack.top());
                                {
                                    mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(hand));
                                    mvStack.push(mvStack.top());
                                    {
                                        zrotHand = M_PI/2.0;
                                        // if (zrotHand > M_PI / 4.0)
                                        //     incHand = -0.01;
                                        // else if (zrotHand < M_PI / -4.0)
                                        //     incHand = 0.01;

                                        mvStack.top() *= glm::rotate(glm::mat4(1.0f), zrotHand, glm::vec3(0.0, 0.0, 1.0));
                                        mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                                        mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 0.1f));

                                        glUniform4f(colorCube, 0.925f, 0.749f, 0.263f, 1.0f);
                                        glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

                                        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
                                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
                                        glEnableVertexAttribArray(0);
                                        glDrawArrays(GL_TRIANGLES, 0, 36);
                                    }
                                    mvStack.pop();
                                }
                                mvStack.pop();
                            }
                            mvStack.pop();
                        }
                        mvStack.pop();
                    }
                    mvStack.pop();
                }
            }
            mvStack.pop();
        }
        mvStack.pop();
    }
    mvStack.pop();
}

void window_size_callback(GLFWwindow *win, int newWidth, int newHeight)
{
    aspect = (float)newWidth / newHeight;
    glViewport(0, 0, newWidth, newHeight);
    //pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

int main(void)
{
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow *window = glfwCreateWindow(900, 600, "brazo robot", NULL, NULL);
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