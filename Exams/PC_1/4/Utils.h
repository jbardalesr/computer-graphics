#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <string>
#include <fstream>

using namespace std;
// Error detection for GLSL
void printShaderLog(GLuint shader)
{   
    int len = 0;
    int chWritten = 0;
    char *log;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

    if (len > 0)
    {
        log = (char *)malloc(len);
        glGetShaderInfoLog(shader, len, &chWritten, log);
        cout << "Shader info log" << log << endl;
        free(log);
    }
}

void printProgramLog(int prog)
{
    int len = 0;
    int chWritten = 0;
    char *log;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
    if (len > 0)
    {
        log = (char *)malloc(len);
        glGetProgramInfoLog(prog, len, &chWritten, log);
        cout << "Program info log: " << log << endl;
        free(log);
    }
}

bool checkOpenGLError()
{
    bool foundError = false;
    int glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        cout << "glError: " << glErr << endl;
        foundError = true;
        glErr = glGetError();
    }
    return foundError;
}

string readShaderSource(const char *filePath)
{
    string content = "";
    ifstream fileStream(filePath, ios::in);
    string line = "";
    while (!fileStream.eof())
    {
        getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}

