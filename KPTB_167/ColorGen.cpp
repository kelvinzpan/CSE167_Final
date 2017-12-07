#include "ColorGen.h"

/*
* Initialize spread of colors
*/
ColorGen::ColorGen(std::vector<glm::vec3> biomeColors, float spread)
{
	this->biomeColors = biomeColors;
	this->spread = spread;
	this->halfSpread = spread / 2.0f;
	this->part = 1.0f / (biomeColors.size() - 1);
}

/*
* Generate colors based on HeightGen heights
*/
std::vector<std::vector<glm::vec3>> ColorGen::generateColors(std::vector<std::vector<float>> heights, float amplitude)
{
	std::vector<std::vector<glm::vec3>> colors(heights.size(), std::vector<glm::vec3>(heights.size(), glm::vec3(0.0f, 0.0f, 0.0f)));

	for (unsigned int z = 0; z < heights.size(); z++)
	{
		for (unsigned int x = 0; x < heights[z].size(); x++)
		{
			colors[z][x] = calcColor(heights[z][x], amplitude);
		}
	}

	return colors;
}

/*
* Generates a color based on a given height
*/
glm::vec3 ColorGen::calcColor(float height, float amplitude)
{
	float value = (height + amplitude) / (amplitude * 2);
	value = glm::clamp((value - halfSpread) * (1.0f / spread), 0.0f, 0.9999f);
	int firstBiome = (int) std::floor (value / part);
	float blend = (value - (firstBiome * part)) / part;
	return ColorGen::interpolateColors(biomeColors[firstBiome], biomeColors[firstBiome + 1], blend);
}

/*
* Average of two colors, so we can have smooth color transitions
*/
glm::vec3 ColorGen::interpolateColors(glm::vec3 c1, glm::vec3 c2, float blend)
{
	float c1Weight = 1.0f - blend;
	float r = (c1Weight * c1.x) + (blend * c2.x);
	float g = (c1Weight * c1.y) + (blend * c2.y);
	float b = (c1Weight * c1.z) + (blend * c2.z);
	return glm::vec3(r, g, b);
}