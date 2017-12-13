#define _CRT_SECURE_NO_WARNINGS

#include "Geode.h"
#include "Window.h"

Geode::Geode(const char * filepath)
{
	// Parse the obj file
	parse(filepath);

	this->dontDraw = false;

	// Construct default material
	material = {
		glm::vec3(0.75164f, 0.60648f, 0.22648f), // color_diff
		glm::vec3(0.628281f, 0.555802f, 0.366065f), // color_spec
		glm::vec3(0.24725f, 0.1995f, 0.0745f), // color_ambi
		0.4f // spec_shine
	};

	// Construct default dir light
	light = {
		glm::vec3(1.0f, 1.0f, 1.0f), // color
		glm::vec3(-1.0f, -0.5f, 0.5f) // direction
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_v);
	glGenBuffers(1, &VBO_n);
	glGenBuffers(1, &EBO);

	// Bind VAO to start binding rest of the buffers
	glBindVertexArray(VAO);

	// Bind VBO for vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_v);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Bind VBO_n for vertex normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_n);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// EBO for indices, so we know order to render
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Unbind current VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Geode::~Geode()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO_v);
	glDeleteBuffers(1, &VBO_n);
	glDeleteBuffers(1, &EBO);
}

void Geode::draw(GLuint program, glm::mat4 C)
{
	if (this->dontDraw) return;

	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * C;
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelview"), 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &C[0][0]);

	// Pass in material values
	glUniform3f(glGetUniformLocation(program, "material.color_diff"), material.color_diff.x, material.color_diff.y, material.color_diff.z);
	glUniform3f(glGetUniformLocation(program, "material.color_spec"), material.color_spec.x, material.color_spec.y, material.color_spec.z);
	glUniform3f(glGetUniformLocation(program, "material.color_ambi"), material.color_ambi.x, material.color_ambi.y, material.color_ambi.z);
	glUniform1f(glGetUniformLocation(program, "material.spec_shine"), material.spec_shine);

	// Pass in light values (directional light)
	glUniform3f(glGetUniformLocation(program, "light.light_color"), light.light_color.x, light.light_color.y, light.light_color.z);
	glUniform3f(glGetUniformLocation(program, "light.light_dir"), light.light_dir.x, light.light_dir.y, light.light_dir.z);
	glUniform3f(glGetUniformLocation(program, "camPos"), Window::currCam->cam_pos.x, Window::currCam->cam_pos.y, Window::currCam->cam_pos.z);
	//std::cout << "(" << Window::cam_pos.x << ", " << Window::cam_pos.y << ", " << Window::cam_pos.z << ")" << std::endl;

	// Now draw the OBJObject. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);

	if (particleActive)
	{
		glUseProgram(Window::particleShaderProgram);
		particleEffect->draw(Window::particleShaderProgram, C);
	}
}

void Geode::update()
{

}

void Geode::activeParticleEffect()
{
	particleActive = !particleActive;
}

void Geode::setParticleEffect()
{
	//only fire for now
	particleEffect = new ParticleSpawn();
}

void Geode::parse(const char * filepath)
{
	FILE* objFile; // File to read
	int c1, c2; // First two chars of each line
	float x, y, z; // Vertex/Normals coordinates
	float r, g, b; // Vertex color
	unsigned int fv1, fn1, fv2, fn2, fv3, fn3; // Faces

	// Populate the face indices, vertices, and normals vectors with the OBJ Object data
	objFile = std::fopen(filepath, "r");
	if (!objFile)
	{
		std::cerr << "error loading file" << std::endl;
		exit(-1);
	}
	else
	{
		while (true)
		{
			c1 = std::fgetc(objFile);
			if (c1 == EOF) // While we haven't reached EOF
			{
				break;
			}
			else if (c1 == '#') // Skip line
			{
				while (c1 != '\n' && c1 != EOF)
				{
					c1 = std::fgetc(objFile);
				}
				continue;
			}
			else if (c1 == 'f') // Evaluate face
			{
				fscanf(objFile, "%u//%u %u//%u %u//%u", &fv1, &fn1, &fv2, &fn2, &fv3, &fn3);
				indices.push_back(fv1 - 1);
				indices.push_back(fv2 - 1);
				indices.push_back(fv3 - 1);
			}
			else if (c1 == 'v')
			{
				c2 = std::fgetc(objFile);

				if (c1 == 'v' && c2 == ' ') // Evaluate vertex
				{
					fscanf(objFile, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
					vertices.push_back(glm::vec3(x, y, z));

					// Get min and max vertices
					if (x > maxCoord.x)
					{
						maxCoord.x = x;
					}
					if (y > maxCoord.y)
					{
						maxCoord.y = y;
					}
					if (z > maxCoord.z)
					{
						maxCoord.z = z;
					}
					if (x < minCoord.x)
					{
						minCoord.x = x;
					}
					if (y < minCoord.y)
					{
						minCoord.y = y;
					}
					if (z < minCoord.z)
					{
						minCoord.z = z;
					}
				}
				else if (c1 == 'v' && c2 == 'n') // Evaluate normal
				{
					fscanf(objFile, "%f %f %f", &x, &y, &z);
					normals.push_back(glm::vec3(x, y, z));
				}
			}
		}
	}
	std::cout << "\nParsing of complete.\n";
	std::cout << "Vertices: " << vertices.size() << ", normals: " << normals.size() << ", faces: " << indices.size() / 3 << "\n\n";
}

// Call setParentMT before using
// Should be called after initializating the Geode. Will center model to origin and scale to desired max length.
// If centeredOnFloor is true, will center at origin ABOVE the XZ plane (Y >= 0)
void Geode::initSize(float scale, bool centeredOnFloor)
{
	float xLen = this->maxCoord.x - this->minCoord.x;
	float yLen = this->maxCoord.y - this->minCoord.y;
	float zLen = this->maxCoord.z - this->minCoord.z;

	// Scale so maximum dimension matches parameter scale
	float scaleFactor = scale / std::max(std::max(xLen, yLen), zLen);
	this->parentMT->scaleOnce(glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor, scaleFactor, scaleFactor)));

	// Move to origin
	float midX = (this->maxCoord.x + this->minCoord.x) / 2 * scaleFactor;
	float midY = (this->maxCoord.y + this->minCoord.y) / 2 * scaleFactor;
	float midZ = (this->maxCoord.z + this->minCoord.z) / 2 * scaleFactor;
	this->parentMT->translateOnce(glm::translate(glm::mat4(1.0f), glm::vec3(-midX, -midY, -midZ)));

	// Update class variables to match
	this->maxCoord *= glm::vec3(scaleFactor, scaleFactor, scaleFactor);
	this->minCoord *= glm::vec3(scaleFactor, scaleFactor, scaleFactor);
	this->maxCoord += glm::vec3(-midX, -midY, -midZ);
	this->minCoord += glm::vec3(-midX, -midY, -midZ);

	if (centeredOnFloor)
	{
		// Translate so feet are at origin (no negative Y)
		this->parentMT->translateOnce(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, midY, 0.0f)));

		// Update class variables to match
		this->maxCoord += glm::vec3(0.0f, midY, 0.0f);
		this->minCoord += glm::vec3(0.0f, midY, 0.0f);
	}
}

// Sets pointer to the parent MT
void Geode::setParentMT(MatrixTransform * parent)
{
	this->parentMT = parent;
}