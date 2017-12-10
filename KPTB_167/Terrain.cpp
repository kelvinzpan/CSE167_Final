#include "Terrain.h"
#include "Window.h"
#include "stb_image.h"

const double PERSIST = 0.4;
const double FREQ = 0.03;
const double AMP = 169.0;
const double AMP_OFFSET = 42.0;
const int OCT = 4;

const float SIZE = 2000.0f;

std::vector<glm::vec3> COLORS = // Lowest to highest
{
	glm::vec3(0.5f, 0.05f, 0.1f), // red
	glm::vec3(0.412f, 0.412f, 0.412f) * 0.2f, // grey
	glm::vec3(1.0f, 0.96f, 0.96f) // white
};

std::vector<std::string> TEXTURES = // Lowest to highest
{
	"res/textures/groundTex.png",
	"res/textures/darkGrassTex.png",
	"res/textures/snowTex.png"
};

/*
 * Class constructor, use default seed if seed == -1
 */
Terrain::Terrain(int gridSize, int seed)
{
	this->gridSize = gridSize;
	this->useFlatColor = false;
	this->seed = (seed == -1) ? 420 : seed;

	heightGen = new PerlinNoise(PERSIST, FREQ, AMP, OCT, this->seed);

	// Default dir light taken from Geode.cpp
	this->terrainLight = {
		glm::vec3(1.0f, 1.0f, 1.0f), // color
		glm::vec3(-1.0f, -0.5f, 0.5f) // direction
	};

	this->heights = Terrain::generateHeights(this->gridSize, this->heightGen);

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

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO_v);
	glDeleteBuffers(1, &VBO_n);
	glDeleteBuffers(1, &VBO_c);
	glDeleteBuffers(1, &EBO);
}

/*
 * Alternate boolean that swaps betweens flat colors and textures.
 */
void Terrain::swapColors()
{
	this->useFlatColor = !this->useFlatColor;
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
std::vector<std::vector<float>> Terrain::generateHeights(int gridSize, PerlinNoise * heightGen)
{
	std::vector<std::vector<float>> heights(gridSize + 1, std::vector<float>(gridSize + 1, 0.0f));
	for (unsigned int z = 0; z < heights.size(); z++)
	{
		for (unsigned int x = 0; x < heights[z].size(); x++)
		{
			heights[z][x] = heightGen->GetHeight(x, z) + AMP_OFFSET;
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
			int topLeft = (col * vertexCount) + row;
			int topRight = topLeft + 1;
			int bottomLeft = ((col + 1) * vertexCount) + row;
			int bottomRight = bottomLeft + 1;
			newIndices[pointer++] = topLeft;
			newIndices[pointer++] = bottomLeft;
			newIndices[pointer++] = topRight;
			newIndices[pointer++] = topRight;
			newIndices[pointer++] = bottomLeft;
			newIndices[pointer++] = bottomRight;
		}
	}
	return newIndices;
}

/*
 * Calculate a normal given a height
 */
glm::vec3 Terrain::calculateNormal(int x, int z, std::vector<std::vector<float>> &heights)
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
float Terrain::getHeight(unsigned int x, unsigned int z, std::vector<std::vector<float>> &heights)
{
	x = x < 0 ? 0 : x;
	z = z < 0 ? 0 : z;
	x = x >= heights.size() ? heights.size() - 1 : x;
	z = z >= heights.size() ? heights.size() - 1 : z;
	return heights[z][x];
}

/*
* Get the color from given height
*/
glm::vec3 calculateColor(float height, float amplitude, std::vector<glm::vec3> &colors)
{
	float part = 1.0f / (colors.size() - 1);
	float value = (height + amplitude) / (amplitude * 2);
	//value = glm::clamp((value - amplitude / 2.0f) * (1.0f / amplitude), 0.0f, 0.9999f);
	int firstBiome = (int)std::floor(value / part);
	float blend = (value - (firstBiome * part)) / part;

	glm::vec3 c1 = colors[firstBiome];
	glm::vec3 c2 = colors[firstBiome + 1];
	float c1Weight = 1.0f - blend;
	float r = (c1Weight * c1.x) + (blend * c2.x);
	float g = (c1Weight * c1.y) + (blend * c2.y);
	float b = (c1Weight * c1.z) + (blend * c2.z);
	return glm::vec3(r, g, b);
}

/*
 * Generate buffers for vertices, normals, and textures 
 */
void Terrain::generateBuffers()
{
	for (unsigned int z = 0; z < this->heights.size(); z++) {
		for (unsigned int x = 0; x < this->heights[z].size(); x++) {
			this->vertices.push_back(glm::vec3( (float) x / (this->gridSize - 1) * SIZE - SIZE / 2.0f, 
											    this->heights[z][x],
												(float) z / (this->gridSize - 1) * SIZE - SIZE / 2.0f) );
			//std::cout << "(" << vertices[vertices.size() - 1].x << ", " << vertices[vertices.size() - 1].y << ", " << vertices[vertices.size() - 1].z << ")" << std::endl;
			this->normals.push_back(calculateNormal(x, z, this->heights));
			this->colors.push_back(calculateColor(this->heights[z][x], AMP, COLORS));
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
	glBufferData(GL_ARRAY_BUFFER, this->colors.size() * sizeof(glm::vec3), this->colors.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unsigned int Terrain::loadTexture(std::string filepath)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	int width, height, nrChannels;

	unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Cubemap texture failed to load at path: " << filepath << std::endl;
		stbi_image_free(data);
	}
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
}