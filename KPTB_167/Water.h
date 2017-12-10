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

	const GLfloat vertices[8][3] = {
		// "Front" vertices
		{ -1200.0, -1200.0,  1200.0 },{ 1200.0, -1200.0,  1200.0 },{ 1200.0,  1200.0,  1200.0 },{ -1200.0,  1200.0,  1200.0 },
		// "Back" vertices
		{ -1200.0, -1200.0, -1200.0 },{ 1200.0, -1200.0, -1200.0 },{ 1200.0,  1200.0, -1200.0 },{ -1200.0,  1200.0, -1200.0 }
	};

	// Note that GL_QUADS is deprecated in modern OpenGL (and removed from OSX systems).
	// This is why we need to draw each face as 2 triangles instead of 1 quadrilateral
	const GLuint indices[6][6] = {
		// Front face
		{ 0, 1, 2, 2, 3, 0 },
		// Top face
		{ 1, 5, 6, 6, 2, 1 },
		// Back face
		{ 7, 6, 5, 5, 4, 7 },
		// Bottom face
		{ 4, 0, 3, 3, 7, 4 },
		// Left face
		{ 4, 5, 1, 1, 0, 4 },
		// Right face
		{ 3, 2, 6, 6, 7, 3 }
	};

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