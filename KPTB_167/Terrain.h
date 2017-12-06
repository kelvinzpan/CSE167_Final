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

#include "Geode.h"
#include "HeightGen.hpp"
#include "ColorGen.hpp"

class Terrain
{
public:
	static const float SIZE;
	static const int VERTEX_COUNT;

	float x;
	float z;
	
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;

	GLuint VAO, VBO_v, VBO_n, EBO;
	Light light;

	HeightGen * heightGen;
	ColorGen * colorGen;

	Terrain();
	Terrain(int gridX, int gridZ);
	~Terrain();

	std::vector<std::vector<float>> generateHeights(int gridSize, HeightGen * heightGen);
};

#endif