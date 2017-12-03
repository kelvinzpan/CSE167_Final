#ifndef _GROUP_H_
#define _GROUP_H_

#include "Node.h"

// Group for creating groups to transform together
class Group: public Node
{
public:
	std::vector<Node *> childList;

	void addChild(Node * child)
	{
		childList.push_back(child);
	};

	void draw(GLuint program, glm::mat4 C)
	{
		for (auto node : childList)
		{
			node->draw(program, C); 
		}
	};

	void update()
	{
		for (auto node : childList)
		{
			node->update();
		}
	}
};

#endif

