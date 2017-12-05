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

class ParticleSpawn
{
public:
	struct Particle {
		glm::vec3 pos, speed;
		unsigned char r, g, b, a; // Color
		float size, angle, weight;
		float life; // Remaining life of the particle. if < 0 : dead and unused.
		float camDistance;

		Particle()
		{
			pos = glm::vec3(0.0f, 0.0f, 0.0f);
			speed = glm::vec3(1.25f, 1.25f, 1.25f);
			size = 1.0f;
			angle = 0.0f;
			weight = 0.0f;
			life = 1.0f;
			camDistance = 0.0f;

			r = 1.0f;
			g = 0.25f;
			b = 0.5f;
		}

		bool operator<(Particle& that) {
			return this->camDistance > that.camDistance;
		}
	};

	const GLfloat vertices[4][3]= {
		{-0.5f, -0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
		{-0.5f, 0.5f, 0.0f},
		{0.5f, 0.5f, 0.0f}
	};


	GLuint VAO, VBO_vert, VBO_pos, VBO_col;
	const int maxParticles = 500;
	int particleCount = 0;
	int lastUsed = 0;

	std::vector<Particle> pContainer;
	GLfloat particle_pos[4 * 500];
	GLubyte particle_color[4 * 500];

	ParticleSpawn();
	void draw();
	void initializeArrays();
	void updateLiveParticles(int & particleCount);
	int findUnusedParticle();
	~ParticleSpawn();
};

#endif
