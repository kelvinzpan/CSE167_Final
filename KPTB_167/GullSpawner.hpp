#ifndef _GULLSPAWNER_H_
#define _GULLSPAWNER_H_

#include "Geode.h"
#include "Terrain.h"

class GullSpawner
{
public:
	Group* gullGroup;
	Group* baseGull;
	int count;
	float height;
	float speed;

	// Control each gull, but only have one actual instance
	std::vector<MatrixTransform*> flock;
	std::vector<glm::vec3> flockDirs;
	std::vector<int> flockDirCount;
	int maxDirCount = 1000;

	GullSpawner(Group* gullGroup, Group* baseGull, int count, float height, float speed)
	{
		this->gullGroup = gullGroup;
		this->baseGull = baseGull;
		this->count = count;
		this->height = height;
		this->speed = speed;

		this->flock = std::vector<MatrixTransform*>(this->count);
		this->flockDirs = std::vector<glm::vec3>(this->count);
		this->flockDirCount = std::vector<int>(this->count);

		for (int i = 0; i < count; i++)
		{
			MatrixTransform* gullCont = new MatrixTransform();
			this->gullGroup->addChild(gullCont);
			gullCont->addChild(this->baseGull);

			flock[i] = gullCont;
			flockDirs[i] = glm::vec3(0.0f, 0.0f, 1.0f); // Gull model starts facing +z

			// Translate gull to random xz within boundaries, y to input height

			float r = ((float)rand() / (RAND_MAX)); // between 0 and 1
			float xDist = r * Terrain::SIZE - Terrain::SIZE / 2.0f;
			r = ((float)rand() / (RAND_MAX)); // between 0 and 1
			float zDist = r * Terrain::SIZE - Terrain::SIZE / 2.0f;
			gullCont->translateOnce(glm::translate(glm::mat4(1.0f), glm::vec3(xDist, this->height, zDist)));

			// Rotate the gull direction at random
			this->rotateGull(i, 360.0f);

			r = ((float)rand() / (RAND_MAX)); // between 0 and 1
			int dc = r * this->maxDirCount * 2 - this->maxDirCount / 2;
			this->flockDirCount[i] = dc;
		}
	}

	void rotateGull(int gullIndex, float range)
	{
		float r = ((float)rand() / (RAND_MAX)); // between 0 and 1
		float randAng = glm::radians(r * range);
		if (this->flockDirCount[gullIndex] < 0) randAng *= -1;
		this->flockDirCount[gullIndex]++;
		if (this->flockDirCount[gullIndex] > this->maxDirCount) this->flockDirCount[gullIndex] = -this->maxDirCount;
		glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), randAng, glm::vec3(0.0f, 1.0f, 0.0f));

		this->flock[gullIndex]->rotateOnce(rotMat);
		this->flockDirs[gullIndex] = glm::vec3(rotMat * glm::vec4(flockDirs[gullIndex], 1.0f));
	}

	void moveGulls()
	{
		// Move gulls forward
		for (int i = 0; i < this->count; i++)
		{
			glm::vec3 dir = glm::normalize(this->flockDirs[i]) * this->speed;
			this->flock[i]->translateOnce(glm::translate(glm::mat4(1.0f), dir));
			this->rotateGull(i, 0.5f);

			float limits = Terrain::SIZE / 2.0f;

			// If moving outside the map, flip direction
			if (flock[i]->newMat[3][0] < -limits || flock[i]->newMat[3][0] > limits ||
				flock[i]->newMat[3][2] < -limits || flock[i]->newMat[3][2] > limits)
			{
				this->flock[i]->translateOnce(glm::translate(glm::mat4(1.0f), -2.0f * dir));
				this->flock[i]->rotateOnce(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
				this->flockDirs[i] *= -2.0f;
			}
		}
	}
};

#endif