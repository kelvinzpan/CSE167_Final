#ifndef _COLORGEN_HPP_
#define _COLORGEN_HPP_

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

class ColorGen
{
public:
	float spread; // How much of the possible altitudes should be colored
	float halfSpread;

	std::vector<glm::vec3> biomeColors; // low to high colors
	float part;

	ColorGen(std::vector<glm::vec3> biomeColors, float spread);
	~ColorGen() {};

	std::vector<std::vector<glm::vec3>> generateColors(std::vector<std::vector<float>> heights, float amplitude);
	glm::vec3 calcColor(float height, float amplitude);
	glm::vec3 interpolateColors(glm::vec3 c1, glm::vec3 c2, float blend);
};

#endif