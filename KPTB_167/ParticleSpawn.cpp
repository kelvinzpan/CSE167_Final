#include "ParticleSpawn.h"


ParticleSpawn::ParticleSpawn()
{
	beginTime = glfwGetTime();
	static GLfloat * particle_pos = new GLfloat[maxParticles * 4];
	static GLubyte * particle_color = new GLubyte[maxParticles * 4];
	pContainer = std::vector<Particle*>(maxParticles);
	initializeArrays();
	for (int i = 0; i < maxParticles; i++)
	{
		Particle * p = new Particle();
		pContainer[i] = p;
	}
}

void ParticleSpawn::initializeArrays()
{
	glGenVertexArrays(1, &VAO);
	// Bind VAO to start binding rest of the buffers
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO_vert);
	glGenBuffers(1, &VBO_pos);
	glGenBuffers(1, &VBO_col);

	// Bind VBO for vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vert);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	// EBO for indices, so we know order to render
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_col);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (GLvoid*)0);

	// Unbind current VBO and VAO
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);
}

void ParticleSpawn::generateParticles(int newparticles)
{
	for (int i = 0; i < newparticles; i++)
	{
		int particleIndex = findUnusedParticle();
		pContainer[particleIndex]->life = 5.0f; // This particle will live 5 seconds.
		pContainer[particleIndex]->pos = glm::vec3(0, 0, -20.0f);

		float spread = 1.5f;
		glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
		glm::vec3 randomdir = glm::vec3(
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f
		);

		pContainer[particleIndex]->speed = maindir + randomdir*spread;

		// Very bad way to generate a random color
		pContainer[particleIndex]->r = rand() % 256;
		pContainer[particleIndex]->g = rand() % 256;
		pContainer[particleIndex]->b = rand() % 256;
		pContainer[particleIndex]->a = (rand() % 256) / 3;

		pContainer[particleIndex]->size = (rand() % 1000) / 2000.0f + 0.1f;
	}
}

void ParticleSpawn::updateLiveParticles(int & particleCount, double delta)
{
	for (int i = 0; i < maxParticles; i++)
	{
		Particle * p = pContainer[i]; // shortcut

		if (p->life > 0.0f) {

			// Decrease life
			p->life -= delta;
			if (p->life > 0.0f) {

				// Simulate simple physics : gravity only, no collisions
				p->speed += glm::vec3(0.0f, -9.81f, 0.0f)  * (float)delta * 0.5f;
				p->pos += p->speed * (float)delta;
				p->camDistance = pow(glm::length(p->pos - Window::cam_pos), 2);
		
				// Fill the GPU buffer
				particle_pos[4 * particleCount + 0] = p->pos.x;
				particle_pos[4 * particleCount + 1] = p->pos.y;
				particle_pos[4 * particleCount + 2] = p->pos.z;
				//std::cout << p.pos.x << " " << p.pos.y << " " << p.pos.z << std::endl;
				particle_pos[4 * particleCount + 3] = p->size;

				particle_color[4 * particleCount + 0] = p->r;
				particle_color[4 * particleCount + 1] = p->g;
				particle_color[4 * particleCount + 2] = p->b;
				particle_color[4 * particleCount + 3] = p->a;

			}
			else {
				// Particles that just died will be put at the end of the buffer in SortParticles();
				p->camDistance = -1.0f;
			}

			particleCount++;

		}
	}
}

void ParticleSpawn::draw()
{
	double currentTime = glfwGetTime();
	double delta = currentTime - beginTime;
	beginTime = currentTime;

	int newparticles = (int)(delta*10000.0);
	if (newparticles > (int)(0.016f*10000.0))
		newparticles = (int)(0.016f*10000.0);

	generateParticles(newparticles);

	int particleCount = 0;
	updateLiveParticles(particleCount, delta);

	//sort them to optimize traversal and also to properly display by z-value
	std::sort(pContainer.begin(),pContainer.end());

	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(GLfloat) * 4, particle_pos);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_col);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(GLubyte) * 4, particle_color);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vert);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_col);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (GLvoid*)0);

	//glBindBuffer()
	//glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	//glDrawElementsInstanced(GL_TRIANGLE_STRIP, 0, GL_FLOAT, 0, particleCount);
	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
	glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 4, particleCount);

	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	std::cout << particleCount << std::endl;

}

int ParticleSpawn::findUnusedParticle()
{
	for (int i = lastUsed; i< maxParticles; i++) {
		if (pContainer[i]->life < 0) {
			lastUsed = i;
			return i;
		}
	}

	for (int i = 0; i< lastUsed; i++) {
		if (pContainer[i]->life < 0) {
			lastUsed = i;
			return i;
		}
	}

	return 0;
}

ParticleSpawn::~ParticleSpawn()
{
	for (auto e : pContainer)
		delete(e);
}
