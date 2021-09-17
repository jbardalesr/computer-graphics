#include <cmath>
#include <vector>
#include <glm/glm.hpp>

class Cone
{
private:
	int numVertices;
	int numIndices;
    int h;
	std::vector<int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	void init(int);
	float toRadians(float degrees);

public:
	Cone();
	Cone(int prec);
	int getNumVertices();
	int getNumIndices();
	std::vector<int> getIndices();
	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec2> getTexCoords();
	std::vector<glm::vec3> getNormals();
	std::vector<glm::vec3> getTangents();
};