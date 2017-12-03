#include "MatrixTransform.h"

MatrixTransform::MatrixTransform()
{
	transMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
}

void MatrixTransform::addChild(Node * child)
{
	childList.push_back(child);
};

void MatrixTransform::draw(GLuint program, glm::mat4 C)
{
	newMat = C * M;

	for (auto node : childList)
	{
		node->draw(program, newMat);
	}
}

void MatrixTransform::update()
{
	// Transform the M
	M = transMat * M * rotMat * scaleMat;

	// Update children
	for (auto node : childList)
	{
		node->update();
	}
}

// One-time transformations

void MatrixTransform::translateOnce(glm::mat4 trans)
{
	M = trans * M;
}

void MatrixTransform::rotateOnce(glm::mat4 rot)
{
	M = M * rot;
}

void MatrixTransform::scaleOnce(glm::mat4 scale)
{
	M = M * scale;
}

// Looping transformations

void MatrixTransform::setTransMat(glm::mat4 trans)
{
	transMat = trans;
}

void MatrixTransform::setRotMat(glm::mat4 rot)
{
	rotMat = rot;
}

void MatrixTransform::setScaleMat(glm::mat4 scale)
{
	scaleMat = scale;
}