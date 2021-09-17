#include <cmath>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "Cone.h"

using namespace std;

Cone::Cone() {
	init(48, 2);
}

Cone::Cone(int prec) {
	init(prec, 2);
}
Cone::Cone(int prec, int hg) {
	init(prec, hg);
}

float Cone::toRadians(float degrees) { return (degrees  * 3.14159f) / 180.0f; }

void Cone::init(int prec, int hg) {
	h = hg;
	numVertices = prec * 2 + 1;
	numIndices = prec * 6;
	for (int i = 0; i < numVertices; i++) { vertices.push_back(glm::vec3()); }
	for (int i = 0; i < numVertices; i++) { normals.push_back(glm::vec3()); }
	for (int i = 0; i < numVertices; i++) { tangents.push_back(glm::vec3()); }
	for (int i = 0; i < numIndices; i++) { indices.push_back(0); }

		// calculate triangle vertices
	for (int i = 0; i <= 2; i++) {
		for (int j = 0; j <= prec; j++) {

			float y = (i < 1) ? 1.0 : -1.0;
			float x = (i == 0 || i == 2) ? (0) : -(float)cos(toRadians(j*360.0f / prec));
			float z = (i == 0 || i == 2) ? (0) :(float)sin(toRadians(j*360.0f / (float)(prec)));
			
			vertices[i*(prec + 1) + j] = glm::vec3(x, y, z);
			texCoords[i*(prec + 1) + j] = glm::vec2(((float)j / prec), ((float)i / 4));
			normals[i*(prec + 1) + j] = glm::vec3(x, y, z);

			// calculate tangent vector
			if (((x == 0) && (y == 1) && (z == 0)) || ((x == 0) && (y == -1) && (z == 0))) {
				tangents[i*(prec + 1) + j] = glm::vec3(0.0f, 0.0f, -1.0f);
			}
			else {
				tangents[i*(prec + 1) + j] = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z));
			}
		}
	}
	
	// calculate triangle indices
	for (int i = 0; i<2; i++) {
		for (int j = 0; j<prec; j++) {
			indices[6 * (i*prec + j) + 0] = i*(prec + 1) + j;
			indices[6 * (i*prec + j) + 1] = i*(prec + 1) + j + 1;
			indices[6 * (i*prec + j) + 2] = (i + 1)*(prec + 1) + j;
			indices[6 * (i*prec + j) + 3] = i*(prec + 1) + j + 1;
			indices[6 * (i*prec + j) + 4] = (i + 1)*(prec + 1) + j + 1;
			indices[6 * (i*prec + j) + 5] = (i + 1)*(prec + 1) + j;
		}
	}
}
int Cone::getNumVertices() { return numVertices; }
int Cone::getNumIndices() { return numIndices; }
std::vector<int> Cone::getIndices() { return indices; }
std::vector<glm::vec3> Cone::getVertices() { return vertices; }
std::vector<glm::vec2> Cone::getTexCoords() { return texCoords; }
std::vector<glm::vec3> Cone::getNormals() { return normals; }
std::vector<glm::vec3> Cone::getTangents() { return tangents; }