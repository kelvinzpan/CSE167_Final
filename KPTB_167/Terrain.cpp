#include "Terrain.h"

const float Terrain::SIZE = 800.0f;
const int Terrain::VERTEX_COUNT = 128;

/*
 * Class constructor
 */
Terrain::Terrain(int gridX, int gridZ)
{
	this->x = gridX * Terrain::SIZE;
	this->z = gridZ * Terrain::SIZE;
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



