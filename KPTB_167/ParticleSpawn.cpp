#include "ParticleSpawn.h"

ParticleSpawn::ParticleSpawn()
{
	pContainer = std::vector<Particle>(maxParticles);
	initializeArrays();

}

void ParticleSpawn::initializeArrays()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_vert);
	glGenBuffers(1, &VBO_pos);
	glGenBuffers(1, &VBO_col);

	// Bind VAO to start binding rest of the buffers
	glBindVertexArray(VAO);

	// Bind VBO for vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vert);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	// Bind VBO_n for vertex normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	// EBO for indices, so we know order to render
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_col);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (GLvoid*)0);

	// Unbind current VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ParticleSpawn::updateLiveParticles(int & particleCount)
{
	for (int i = 0; i < maxParticles; i++)
	{
		Particle p = pContainer[i]; // shortcut

		if (p.life > 0.0f) {

			// Decrease life
			float r = (rand() % 10) / 10;
			p.life -= r;
			if (p.life > 0.0f) {

				// Simulate simple physics : gravity only, no collisions
				p.speed += glm::vec3(0.0f, -9.81f, 0.0f)  * r * 0.5f;
				p.pos += p.speed * r;
				//p.camDistance = pow(glm::length(p.pos - Window::cam_pos), 2);
				//ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

				// Fill the GPU buffer
				particle_pos[4 * particleCount + 0] = p.pos.x;
				particle_pos[4 * particleCount + 1] = p.pos.y;
				particle_pos[4 * particleCount + 2] = p.pos.z;

				particle_pos[4 * particleCount + 3] = p.size;

				particle_color[4 * particleCount + 0] = p.r;
				particle_color[4 * particleCount + 1] = p.g;
				particle_color[4 * particleCount + 2] = p.b;
				particle_color[4 * particleCount + 3] = p.a;

			}
			else {
				// Particles that just died will be put at the end of the buffer in SortParticles();
				p.camDistance = -1.0f;
			}

			particleCount++;

		}
	}
}

void ParticleSpawn::draw()
{
	int particleCount = 0;
	for (int i = 0; i < maxParticles; i++)
	{
		Particle p = pContainer[i]; // shortcut

		if (p.life > 0.0f) {

			// Decrease life
			float r = (float)(rand() % 10) / 10;
			p.life -= r;
			if (p.life > 0.0f) {

				// Simulate simple physics : gravity only, no collisions
				p.speed += glm::vec3(0.0f, -9.81f, 0.0f)  * r * 0.5f;
				p.pos += p.speed * r;

				// Fill the GPU buffer
				particle_pos[4 * particleCount + 0] = p.pos.x;
				particle_pos[4 * particleCount + 1] = p.pos.y;
				particle_pos[4 * particleCount + 2] = p.pos.z;
				//std::cout << p.pos.x << " " << p.pos.y << " " << p.pos.z << std::endl;
				particle_pos[4 * particleCount + 3] = p.size;

				particle_color[4 * particleCount + 0] = p.r;
				particle_color[4 * particleCount + 1] = p.g;
				particle_color[4 * particleCount + 2] = p.b;
				particle_color[4 * particleCount + 3] = p.a;

			}
			else {
				// Particles that just died will be put at the end of the buffer in SortParticles();
				p.camDistance = -1.0f;
			}

			particleCount++;

		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(GLfloat) * 4, particle_pos);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_col);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(GLubyte) * 4, particle_color);

	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center) -> 1
	glVertexAttribDivisor(2, 1); // color : one per quad -> 1

	// Now draw the OBJObject. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	//glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	glDrawElementsInstanced(GL_TRIANGLE_STRIP, 0, GL_FLOAT, 0, particleCount);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);

	int reborn = findUnusedParticle();
	pContainer[reborn].life = 10;
	pContainer[reborn].pos = glm::vec3(0.0f, 0.0f, 0.0f);
}

int ParticleSpawn::findUnusedParticle()
{
	for (int i = lastUsed; i< maxParticles; i++) {
		if (pContainer[i].life < 0) {
			lastUsed = i;
			return i;
		}
	}

	for (int i = 0; i< lastUsed; i++) {
		if (pContainer[i].life < 0) {
			lastUsed = i;
			return i;
		}
	}

	return 0;
}

ParticleSpawn::~ParticleSpawn()
{

}
