#include "HeightGen.h"

/*
* Constructor with default seed
*/
HeightGen::HeightGen(int octaves, float amplitude, float roughness)
{
	this->seed = 420420;
	this->octaves = octaves;
	this->amplitude = amplitude;
	this->roughness = roughness;
}

/*
* Constructor with given seed
*/
HeightGen::HeightGen(int seed, int octaves, float amplitude, float roughness)
{
	this->seed = seed;
	this->octaves = octaves;
	this->amplitude = amplitude;
	this->roughness = roughness;
}

/*
* Uses getInterpolatedNoise() to get height values,
* and layers additional heights for noise/roughness
*/
float HeightGen::getFinalNoise(int x, int y)
{
	float total = 0;
	float d = (float)pow(2, octaves - 1);

	for (int i = 0; i < octaves; i++) {
		float freq = (float)(pow(2, i) / d);
		float amp = (float)pow(roughness, i) * amplitude;
		total += getInterpolatedNoise(x * freq, y * freq) * amp;
	}

	return total;
}

/*
* Smooths height from getSmoothNoise() by interpolating nearby coords
*/
float HeightGen::getInterpolatedNoise(float x, float y)
{
	int intX = (int)x;
	float fracX = x - intX;
	int intY = (int)y;
	float fracY = y - intY;

	float v1 = HeightGen::getSmoothNoise(intX, intY);
	float v2 = HeightGen::getSmoothNoise(intX + 1, intY);
	float v3 = HeightGen::getSmoothNoise(intX, intY + 1);
	float v4 = HeightGen::getSmoothNoise(intX + 1, intY + 1);
	float i1 = HeightGen::interpolate(v1, v2, fracX);
	float i2 = HeightGen::interpolate(v3, v4, fracX);
	return HeightGen::interpolate(i1, i2, fracY);
}

/*
* Cosine interpolation for even smoother curves between points
*/
float HeightGen::interpolate(float a, float b, float blend)
{
	double theta = blend * M_PI;
	float f = (float)((1.0f - cos(theta)) * 0.5f);
	return a * (1.0f - f) + b * f;
}

/*
* Smooths height from getNoise() by sampling nearby coords
*/
float HeightGen::getSmoothNoise(int x, int y)
{
	float corners = (getNoise(x - 1, y - 1) + getNoise(x + 1, y - 1) + getNoise(x - 1, y + 1)
		+ getNoise(x + 1, y + 1)) / 16.0f;
	float sides = (getNoise(x - 1, y) + getNoise(x + 1, y) + getNoise(x, y - 1) + getNoise(x, y + 1)) / 8.0f;
	float center = getNoise(x, y) / 4.0f;
	return corners + sides + center;
}

/*
* Gets the height based on seed and coordinate position
*/
float HeightGen::getNoise(int x, int y)
{
	// Offset x and y by large constant to avoid similar values
	std::srand(this->seed + x * 1234 + y * 4321);

	// Return height between -1 and 1
	return (static_cast<float> (std::rand()) / static_cast <float> (RAND_MAX)) * 2.0f - 1.0f;
}