#ifndef _HEIGHTGEN_HPP_
#define _HEIGHTGEN_HPP_

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

#define _USE_MATH_DEFINES

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "math.h"

class HeightGen
{
public:
	int seed;
	int octaves;
	float amplitude;
	float roughness;

	HeightGen(int octaves, float amplitude, float roughness);
	HeightGen(int seed, int octaves, float amplitude, float roughness);
	~HeightGen() {};

	float getFinalNoise(int x, int y);
	float getInterpolatedNoise(float x, float y);
	float interpolate(float a, float b, float blend);
	float getSmoothNoise(int x, int y);
	float getNoise(int x, int y);
};

#endif