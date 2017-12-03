#define _CRT_SECURE_NO_WARNINGS

#include "Geode.h"
#include "Window.h"

Geode::Geode(const char * filepath)
{
	// Parse the obj file
	parse(filepath);

	// Construct default material
	material = {
		glm::vec3(0.3f, 0.3f, 0.3f), // color_diff
		glm::vec3(0.1f, 0.1f, 0.1f), // color_spec
		glm::vec3(0.60f, 0.784f, 1.0f), // color_ambi
		.05f * 128.0f // spec_shine
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

	// Now draw the OBJObject. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void Geode::update()
{

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