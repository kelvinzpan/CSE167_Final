#pragma once
#ifndef _PARTICLESPAWN_H_
#define _PARTICLESPAWN_H_

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
#include "Window.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include "Geode.h"

class ParticleSpawn
{
public:
	struct Particle {
		glm::vec3 pos, speed;
		GLubyte r, g, b, a; // Color
		float size, angle, weight;
		float life; // Remaining life of the particle. if < 0 : dead and unused.
		float camDistance;

		Particle()
		{
			pos = glm::vec3(0.0f, 0.0f, 0.0f);
			speed = glm::vec3(0.0f, 0.0f, 0.0f);
			size = 0.0f;
			angle = 0.0f;
			weight = 0.0f;
			life = -1.0f;
			camDistance = -1.0f;
			r = 0;
			g = 0;
			b = 0;
		}

		bool operator<(Particle& that) {
			return this->camDistance > that.camDistance;
		}
	};

	const GLfloat vertices[4][3] = {
		{-0.5f, -0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
		{-0.5f, 0.5f, 0.0f},
		{0.5f, 0.5f, 0.0f}
	};

	GLuint VAO, VBO_vert, VBO_pos, VBO_col;
	GLuint skyVBO, skyVAO;
	GLint shaderProgram;
	const int maxParticles = 500;
	int particleCount = 0;
	int lastUsed = 0;
	double beginTime;

	std::vector<Particle*> pContainer;
	GLfloat particle_pos[4 * 500];
	GLubyte particle_color[4 * 500];

	ParticleSpawn();
	void draw(GLint shader);
	void initializeArrays();
	void generateParticles(int newParticles);
	void updateLiveParticles(int & particleCount, double delta);
	int findUnusedParticle();
	~ParticleSpawn();

};

#endif