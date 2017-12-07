#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "HeightGen.h"
#include "ColorGen.h"
#include "Geode.h"

class Terrain
{
public:
	int gridSize;

	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> textures;

	GLuint VAO, VBO_v, VBO_n, VBO_c, EBO;
	Light terrainLight;

	HeightGen * heightGen;
	std::vector<std::vector<float>> heights;

	ColorGen * colorGen;
	std::vector<std::vector<glm::vec3>> colors;

	Terrain();
	Terrain(int gridSize, int seed); // Use default seed if seed == -1
	~Terrain();

	void draw(GLuint program, glm::mat4 C);
	
	std::vector<std::vector<float>> generateHeights(int gridSize, HeightGen * heightGen);
	std::vector<unsigned int> generateIndices(int vertexCount);
	glm::vec3 calculateNormal(int x, int z, std::vector<std::vector<float>> heights);
	float getHeight(unsigned int x, unsigned int z, std::vector<std::vector<float>> heights);
	void generateBuffers();
	void loadBuffers();
};

#endif