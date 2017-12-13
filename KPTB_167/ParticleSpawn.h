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
//#include "Window.h"
#include <vector>
#include <iostream>
#include <algorithm>

class ParticleSpawn
{
public:
	struct Particle {
		glm::vec3 pos, speed;
		float r, g, b, a; // Color
		float life; // Remaining life of the particle. if < 0 : dead and unused.
		float cameradistance = 0;
		Particle()
		{
			pos = glm::vec3(0.0f, 0.0f, 0.0f);
			speed = glm::vec3(0.0f, 0.0f, 0.0f);

			life = 0.0f;
			r = 0;
			g = 0;
			b = 0;
		}

		bool operator<(Particle& that) {
			// Sort in reverse order : far particles drawn first.
			return this->cameradistance > that.cameradistance;
		}

	};

	ParticleSpawn(int type);
	unsigned int loadTexture(char * path);
	void draw(GLint shader, glm::mat4 c);
	void generateParticles(int newParticles);
	void generateOneTime(int newParticles);
	void updateLiveParticles(double delta);
	void modelTransposeViewRotation(glm::vec3 pos);
	void setParticleType(int type);
	void SortParticles();
	int findUnusedParticle();
	~ParticleSpawn();

	GLuint VAO, VBO;
	GLuint textureID;
	GLint shaderProgram;
	const int maxParticles = 3000;
	int particleCount = 0;
	int lastUsed = 0;
	double beginTime;
	glm::mat4 toWorld; 

	float spawnRate = 200.0f;
	glm::vec3 maindir = glm::vec3(0.0f, 3.0f, 0.0f);
	float updateX;
	float updateY;
	float updateZ;
	float spread;
	float lifeLength;
	bool isFire;
	bool isRevFire;
	bool isExplosion;
	bool showParticleCount;

	std::vector<Particle*> pContainer;
	GLfloat particle_pos[4 * 3000];
	GLubyte particle_color[4 * 3000];

};

#endif
