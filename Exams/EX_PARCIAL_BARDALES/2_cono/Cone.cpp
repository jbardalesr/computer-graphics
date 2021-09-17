#include <cmath>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "Cone.h"
using namespace std;

Cone::Cone()
{
    init(48);
}

Cone::Cone(int prec)
{
    init(prec);
}

float Cone::toRadians(float degrees)
{
    return (degrees * 2.0f * 3.14159f) / 360.0f;
}

void Cone::init(int prec)
{
    numVertices = (prec + 1) * (prec + 1);
    numIndices = prec * prec * 6;
    for (int i = 0; i < numVertices; i++)
    {
        vertices.push_back(glm::vec3());
        texCoords.push_back(glm::vec2());
    }
    for (int i = 0; i < numIndices; i++)
    {
        indices.push_back(0);
    }

    for (int i = 0; i <= prec; i++)
    {
        for (int j = 0; j <= prec; j++)
        {
            // Cono
            float phi = toRadians(j * 360.0f / (float)prec); // 0 <= φ <= 2π
            // el radio del cono se va reduciendo cuando i crece, hasta ser 0 en la punta
            float R = (1.0 -  i / (float)prec);
            float z = R*cos(phi);
            float x = R*sin(phi);
            float y = 2.0 * i / (float)prec - 1.0;

            vertices[i * (prec + 1) + j] = glm::vec3(x, y, z);
            texCoords[i * (prec + 1) + j] = glm::vec2(((float)j / prec), ((float)i / prec));
        }
    }

    // Cylinder/Sphere triangle indices
    for (int i = 0; i < prec; i++)
    {
        for (int j = 0; j < prec; j++)
        {
            indices[6 * (i * prec + j) + 0] = i * (prec + 1) + j;
            indices[6 * (i * prec + j) + 1] = i * (prec + 1) + j + 1;
            indices[6 * (i * prec + j) + 2] = (i + 1) * (prec + 1) + j;
            indices[6 * (i * prec + j) + 3] = i * (prec + 1) + j + 1;
            indices[6 * (i * prec + j) + 4] = (i + 1) * (prec + 1) + j + 1;
            indices[6 * (i * prec + j) + 5] = (i + 1) * (prec + 1) + j;
        }
    }
}

int Cone::getNumVertices() { return numVertices; }
int Cone::getNumIndices() { return numIndices; }
std::vector<int> Cone::getIndices() { return indices; }
std::vector<glm::vec2> Cone::getTexCoords() { return texCoords; };
std::vector<glm::vec3> Cone::getVertices() { return vertices; }