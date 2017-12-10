#pragma once
#ifndef _WATER_H_
#define _WATER_H_

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
#include <iostream>
#include <string>
#include "Window.h"

class Water
{
public:
	Water();
	~Water();
	unsigned int loadTexture(char * path);
	void initializeFrameBuffers();
	void bindReflectionBuffer();
	void bindRefractionBuffer();
	void unbindBuffer();
	void draw(GLuint shader, glm::mat4 c);
	void update();

	int REFLECTION_WIDTH = 1920;
	int REFLECTION_HEIGHT = 1080;

	int REFRACTION_WIDTH = 1920;
	int REFRACTION_HEIGHT = 1080;

	GLuint VAO, VBO, EBO;
	GLuint shaderProgram;
	GLuint reflectionFB, refractionFB;
	GLuint reflectionTexture, refractionTexture, dudvMap;
	GLuint reflectDepthBuffer , refractDepthBuffer;
	glm::mat4 toWorld;
	float waterHeight = 0.0f;
	float waveSpeed = 0.03f;
	float moveFactor = 0.0f;
	unsigned int textureID;

	double beginTime, currTime, delta;
};
#endif