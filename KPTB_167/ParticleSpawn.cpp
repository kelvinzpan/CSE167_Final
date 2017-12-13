#include "ParticleSpawn.h"
#include "Window.h"
#include "stb_image.h"

ParticleSpawn::ParticleSpawn(int type)
{
	beginTime = glfwGetTime();
	pContainer = std::vector<Particle*>(maxParticles);

	GLfloat vertices[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	//Fill mess buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Set mesh attrib
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	textureID = loadTexture("res/textures/fire_texture.jpg");
	showParticleCount = false;
	for (int i = 0; i < maxParticles; i++)
	{
		Particle * p = new Particle();
		pContainer[i] = p;
	}
	setParticleType(type);
}
void ParticleSpawn::setParticleType(int type)
{
	if (type == 1)
	{
		isFire = true;
		spawnRate = 200.0f;
		maindir = glm::vec3(0.0f, 3.0f, 0.0f);
		updateX = rand() % 10 / 10.0f ; //could use some work
		updateZ = rand() % 10 / 10.0f ; //could use some work
		updateY = 2.0f;
		spread = 1.0f;
		lifeLength = 3.0f;
	}
	else if(type == 2)
	{
		isExplosion = true;
		maindir = glm::vec3(0.0f, 1.0f, 0.0f);
		updateX = rand() % 10 / 10.0f + 0.2f; //could use some work
		updateZ = rand() % 10 / 10.0f + 0.2f; //could use some work
		updateY = rand() % 10 / 10.0f + 0.2f; //could use some work
		lifeLength = 4.0f;
		generateOneTime(maxParticles); //explosion is just one big NUT and no regen
	}
	else if (type == 3)
	{
		isRevFire = true;
		spawnRate = 450.0f;
		maindir = glm::vec3(-2.0f, -45.0f, 0.0f);
		updateX = -(rand() % 10 / 10.0f); //could use some work
		updateZ = -(rand() % 10 / 10.0f); //could use some work
		updateY = -30.0f;
		spread = 1.0f;
		lifeLength = 3.0f;
	}
}

void ParticleSpawn::generateOneTime(int newparticles)
{
	int negativeCounter = 0;
	lastUsed = 0;
	particleCount = maxParticles;
	for (int i = 0; i < newparticles; i++)
	{
		pContainer[i]->life = lifeLength + ((double)rand() / (RAND_MAX - 1)) - 0.7;
		pContainer[i]->pos = glm::vec3(0, 0, 0.0f);

		float spread = 4.0f;
		
		glm::vec3 randomdir = glm::vec3(
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f
		);
		
		switch (negativeCounter)
		{
		case 0:
			randomdir.x = -randomdir.x;
			break;
		case 1:
			randomdir.z = -randomdir.z;
			break;
		case 2:
			randomdir.x = -randomdir.x;
			randomdir.z = -randomdir.z;
			break;
		case 3: 
			break;
			//both stay positive;
		};
		negativeCounter++;
		negativeCounter = negativeCounter % 4;
		pContainer[i]->speed = maindir + randomdir*spread;

		pContainer[i]->r = 1.0f;
		pContainer[i]->g = 0.0f;
		pContainer[i]->b = 0.0f;
		pContainer[i]->a = 1;
	}
}

void ParticleSpawn::generateParticles(int newparticles)
{
	for (int i = 0; i < newparticles; i++)
	{
		int particleIndex = findUnusedParticle();
		pContainer[particleIndex]->life = lifeLength;
		pContainer[particleIndex]->pos = glm::vec3(0, 0, 0.0f);


		//glm::vec3 maindir = glm::vec3(0.0f, 3.0f, 0.0f);
		glm::vec3 randomdir = glm::vec3(
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f
		);
		if (isRevFire)
			randomdir = -randomdir;

		pContainer[particleIndex]->speed = maindir + randomdir*spread;
		
		pContainer[particleIndex]->r = 1.0f;
		pContainer[particleIndex]->g = 0.0f;
		pContainer[particleIndex]->b = 0.0f;
		pContainer[particleIndex]->a = 1;
	}
}

void ParticleSpawn::updateLiveParticles(double delta)
{
	int negativeCounter = 0;
	for (int i = 0; i < maxParticles; i++)
	{
		Particle * p = pContainer[i]; 

		p->life -= delta;
		if (p->life > 0.0f) {

			if (isExplosion)
			{
				switch (negativeCounter)
				{
				case 0:
					updateX = -updateX + 2.5f;
					updateY = 0;
					updateZ = 0;
					break;
				case 1:
					updateZ = -updateZ + 2.5f;
					updateY = 0;
					updateX = 0;
					break;
				case 2:
					updateX = -updateX + 2.5f;
					updateZ = -updateZ + 2.5f;
					updateY = 0;
					break;
				case 3:
					break;
					//both stay positive;
				};

				negativeCounter++;
				negativeCounter = negativeCounter % 4;
			}

			p->speed += glm::vec3(updateX, updateY, updateZ)  * (float)delta * 0.5f;
			p->pos += p->speed * (float)delta;	
			p->cameradistance = pow(glm::length(p->pos - Window::currCam->cam_pos), 2);

			if (isFire || isRevFire)
			{			
				//simulate fire color
				if (p->life >= 2.625f)
				{
					p->r = 0.886f;
					p->g = 0.345f;
					p->b = 0.133f;
				}
				else if (p->life >= 2.25f)
				{
					p->g = 0.552f;
				}
				else if (p->life >= 1.875f)
				{
					p->g = 0.605f;
				}
				else if (p->life >= 1.5f)
				{
					p->g = 0.656f;
				}
				else if (p->life >= 1.125f)
				{
					p->g = 0.707f;
				}
				else if (p->life >= 0.75f)
				{
					p->g = 0.754f;
				}
				else if (p->life >= 0.375f)
				{
					p->g = 0.806f;
				}
				else if (p->life >= 0.15f)
				{
					p->r = 0.3464f;
					p->g = 0.155f;
					p->b = 0.133f;
				}
				else
				{
					p->r = 0.252f;
					p->g = 0.189f;
					p->b = 0.133f;
				}
			}
			else if(isExplosion)
			{
				if (p->life >= 2.625f)
				{
					p->r = 0.7f;
					p->g = 0.7f;
					p->b = 0.7f;
				}
				else if (p->life >= 2.25f)
				{
					p->r = 0.8f;
					p->g = 0.8f;
					p->b = 0.8f;
				}
				else if (p->life >= 1.875f)
				{
					p->r = 0.85f;
					p->g = 0.85f;
					p->b = 0.85f;
				}
				else if (p->life >= 1.5f)
				{
					p->r = 0.9f;
					p->g = 0.9f;
					p->b = 0.9f;
				}
				else if (p->life >= 1.125f)
				{
					p->r = 0.8f;
					p->g = 0.8f;
					p->b = 0.8f;
				}
				else if (p->life >= 0.75f)
				{
					p->r = 0.5f;
					p->g = 0.5f;
					p->b = 0.5f;
				}
				else if (p->life >= 0.375f)
				{
					p->r = 0.3f;
					p->g = 0.3f;
					p->b = 0.3f;
				}
				else
				{
					p->r = 0.0f;
					p->g = 0.0f;
					p->b = 0.0f;
				}
			}

		}
		else
		{
			p->cameradistance = -1.0f;
		}
	}
}

void ParticleSpawn::SortParticles() {
	std::sort(pContainer.begin(), pContainer.end());
}

void ParticleSpawn::draw(GLint shader, glm::mat4 c)
{
	toWorld = c;
	shaderProgram = shader;
	double currentTime = glfwGetTime();
	double delta = currentTime - beginTime;
	beginTime = currentTime;

	if (isFire || isRevFire)
	{
		int newparticles = (int)(delta*spawnRate);
		if (newparticles > (int)(0.016f*spawnRate))
			newparticles = (int)(0.016f*spawnRate);
		generateParticles(newparticles);
	}

	if (particleCount == 0 && isExplosion)
		return;

	particleCount = 0;
	updateLiveParticles(delta);
	SortParticles();

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &Window::P[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture"), 0);

	for (int i = 0; i < maxParticles; i++)
	{
		Particle * p = pContainer[i];

		if (p->life > 0.0f) {

			//this way the texture isn't 2d. reset toWorld each time
			toWorld = c;
			//then transpose the model matrix with view matrix's rotation, with the position of this instance put in as a translation
			modelTransposeViewRotation(p->pos);
			//this way multiplication clears the rotation matrix, but saves the position of the instance
			glm::mat4 modelview = Window::V * toWorld;

			glUniform3f(glGetUniformLocation(shaderProgram, "instancePos"), p->pos.x, p->pos.y, p->pos.z);
			glUniform4f(glGetUniformLocation(shaderProgram, "instanceColor"), p->r, p->g, p->b, p->a);
			//std::cout << (float)p->r << " " << (float)p->g << " " << (float)p->b << " " << (float) p->a << std::endl;
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelview"), 1, GL_FALSE, &modelview[0][0]);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
			glBindVertexArray(0);
			particleCount++;
		}
	}

	//Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
	
	if(showParticleCount)
		std::cout << "Currently alive particle count: " << particleCount << std::endl;
}

void ParticleSpawn::modelTransposeViewRotation(glm::vec3 pos)
{
	//toWorld = glm::translate(glm::mat4(1.0f), pos) * toWorld;
	toWorld[0][0] = Window::V[0][0];
	toWorld[0][1] = Window::V[1][0];
	toWorld[0][2] = Window::V[2][0];
	toWorld[1][0] = Window::V[0][1];
	toWorld[1][1] = Window::V[1][1];
	toWorld[1][2] = Window::V[2][1];
	toWorld[2][0] = Window::V[0][2];
	toWorld[2][1] = Window::V[1][2];
	toWorld[2][2] = Window::V[2][2];
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
	lastUsed = 0;
	return 0;
}

unsigned int ParticleSpawn::loadTexture(char * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	int width, height, nrChannels;

	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Cubemap texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
}

ParticleSpawn::~ParticleSpawn()
{
	for (auto e : pContainer)
		delete(e);

	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &VBO);
}
