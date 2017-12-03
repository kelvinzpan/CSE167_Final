#ifndef _MATRIXTRANSFORM_H_
#define _MATRIXTRANSFORM_H_

#include "Group.h"

// Group for creating groups to transform together
class MatrixTransform : public Group
{
public:
	MatrixTransform();
	~MatrixTransform() {};

	glm::mat4 M, newMat;
	glm::mat4 transMat, rotMat, scaleMat;
	std::vector<Node *> childList;

	void addChild(Node * child);
	void draw(GLuint program, glm::mat4 C);
	void update();

	void translateOnce(glm::mat4 trans);
	void rotateOnce(glm::mat4 rot);
	void scaleOnce(glm::mat4 scale);

	void setTransMat(glm::mat4 trans);
	void setRotMat(glm::mat4 rot);
	void setScaleMat(glm::mat4 scale);
};

#endif