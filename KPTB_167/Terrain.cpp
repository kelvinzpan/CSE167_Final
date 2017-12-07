#include "Terrain.h"
#include "Window.h"

int OCTAVES = 2;
float AMPLITUDE = 30.0f;
float ROUGHNESS = 1.0f;
std::vector<glm::vec3> COLORS = // Lowest to highest
{
	glm::vec3(0.169f, 0.114f, 0.055f), // dark brown
	glm::vec3(0.133f, 0.545f, 0.133f), // forest green
	glm::vec3(0.412f, 0.412f, 0.412f), // dim grey
	glm::vec3(0.97f, 0.97f, 1.0f) // blu-ish white
};

/*
 * Class constructor, use default seed if seed == -1
 */
Terrain::Terrain(int gridSize, int seed)
{
	this->gridSize = gridSize;
	if (seed == -1)
	{
		heightGen = new HeightGen(OCTAVES, AMPLITUDE, ROUGHNESS);
	}
	else
	{
		heightGen = new HeightGen(seed, OCTAVES, AMPLITUDE, ROUGHNESS);
	}
	colorGen = new ColorGen(COLORS, AMPLITUDE);

	// Default dir light taken from Geode.cpp
	this->terrainLight = {
		glm::vec3(1.0f, 1.0f, 1.0f), // color
		glm::vec3(-1.0f, -0.5f, 0.5f) // direction
	};

	this->heights = Terrain::generateHeights(this->gridSize, this->heightGen);
	this->colors = this->colorGen->generateColors(this->heights, AMPLITUDE);

	this->indices = Terrain::generateIndices(this->heights.size());
	Terrain::generateBuffers();
	Terrain::loadBuffers();
}

/*
 * Destructor
 */
Terrain::~Terrain()
{
	delete(heightGen);
	delete(colorGen);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO_v);
	glDeleteBuffers(1, &VBO_n);
	glDeleteBuffers(1, &VBO_c);
	glDeleteBuffers(1, &EBO);
}

/*
 * Draw the terrain. Intended to call from Window's draw function
 */
void Terrain::draw(GLuint program, glm::mat4 C)
{
	glm::mat4 modelview = Window::V * C;
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelview"), 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &C[0][0]);

	glUniform3f(glGetUniformLocation(program, "light.light_color"), this->terrainLight.light_color.x, this->terrainLight.light_color.y, this->terrainLight.light_color.z);
	glUniform3f(glGetUniformLocation(program, "light.light_dir"), this->terrainLight.light_dir.x, this->terrainLight.light_dir.y, this->terrainLight.light_dir.z);
	glUniform3f(glGetUniformLocation(program, "camPos"), Window::cam_pos.x, Window::cam_pos.y, Window::cam_pos.z);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

/*
 * Generate heights for every vertex of the terrain
 */
std::vector<std::vector<float>> Terrain::generateHeights(int gridSize, HeightGen * heightGen)
{
	std::vector<std::vector<float>> heights(gridSize + 1, std::vector<float>(gridSize + 1, 0.0f));

	for (unsigned int z = 0; z < heights.size(); z++)
	{
		for (unsigned int x = 0; x < heights[z].size(); x++)
		{
			heights[z][x] = heightGen->getFinalNoise(x, z);
		}
	}
	return heights;
}

/*
 * Generate index buffer of terrain made of triangles
 */
std::vector<unsigned int> Terrain::generateIndices(int vertexCount) {
	int indexCount = (vertexCount - 1) * (vertexCount - 1) * 6;
	std::vector<unsigned int> newIndices(indexCount, 0);

	int pointer = 0;
	for (int col = 0; col < vertexCount - 1; col++) {
		for (int row = 0; row < vertexCount - 1; row++) {
			int topLeft = (row * vertexCount) + col;
			int topRight = topLeft + 1;
			int bottomLeft = ((row + 1) * vertexCount) + col;
			int bottomRight = bottomLeft + 1;
			newIndices[pointer++] = topLeft;
			newIndices[pointer++] = bottomLeft;
			newIndices[pointer++] = bottomRight;
			newIndices[pointer++] = topLeft;
			newIndices[pointer++] = bottomRight;
			newIndices[pointer++] = topRight;
		}
	}
	return newIndices;
}

/*
 * Calculate a normal given a height
 */
glm::vec3 Terrain::calculateNormal(int x, int z, std::vector<std::vector<float>> heights)
{
	float heightL = getHeight(x - 1, z, heights);
	float heightR = getHeight(x + 1, z, heights);
	float heightD = getHeight(x, z - 1, heights);
	float heightU = getHeight(x, z + 1, heights);
	glm::vec3 normal = glm::vec3(heightL - heightR, 2.0f, heightD - heightU);
	return glm::normalize(normal);
}

/*
 * Get the height from heights
 */
float Terrain::getHeight(unsigned int x, unsigned int z, std::vector<std::vector<float>> heights)
{
	x = x < 0 ? 0 : x;
	z = z < 0 ? 0 : z;
	x = x >= heights.size() ? heights.size() - 1 : x;
	z = z >= heights.size() ? heights.size() - 1 : z;
	return heights[z][x];
}

/*
 * Generate buffers for vertices, normals, and textures
 */
void Terrain::generateBuffers()
{
	for (unsigned int z = 0; z < this->heights.size(); z++) {
		for (unsigned int x = 0; x < this->heights[z].size(); x++) {
			this->vertices.push_back(glm::vec3(x, this->heights[z][x], z));
			//std::cout << "(" << vertices[vertices.size() - 1].x << ", " << vertices[vertices.size() - 1].y << ", " << vertices[vertices.size() - 1].z << ")" << std::endl;
			this->textures.push_back(this->colors[z][x]);
			//std::cout << "(" << textures[textures.size() - 1].x << ", " << textures[textures.size() - 1].y << ", " << textures[textures.size() - 1].z << ")" << std::endl;
			this->normals.push_back(calculateNormal(x, z, this->heights));
			//std::cout << "(" << normals[normals.size() - 1].x << ", " << normals[normals.size() - 1].y << ", " << normals[normals.size() - 1].z << ")" << std::endl;
		}
	}
}

/*
* Load the generated buffers
*/
void Terrain::loadBuffers()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_v);
	glGenBuffers(1, &VBO_n);
	glGenBuffers(1, &VBO_c);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_v);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), this->vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_n);
	glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(glm::vec3), this->normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_c);
	glBufferData(GL_ARRAY_BUFFER, this->textures.size() * sizeof(glm::vec3), this->textures.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}