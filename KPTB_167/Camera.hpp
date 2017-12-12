#ifndef _CAMERA_H_
#define _CAMERA_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	glm::vec3 cam_pos;
	glm::vec3 cam_look_at;
	glm::vec3 cam_up;
	glm::vec3 cam_look_dir; // look_at = pos + look_dir

	Camera()
	{
		this->cam_pos = glm::vec3(0.0f, 0.0f, 0.0f);
		this->cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
		this->cam_look_dir = glm::vec3(1.0f, 0.0f, 0.0f);
		this->cam_look_at = glm::vec3(1.0f, 0.0f, 0.0f);
	};

	void setLookDir(glm::vec3 look_dir)
	{
		this->cam_look_dir = look_dir;
		this->cam_look_at = this->cam_pos + this->cam_look_dir;
	}

	void setPos(glm::vec3 pos)
	{
		this->cam_pos = pos;
		this->cam_look_at = this->cam_pos + this->cam_look_dir;
	}
};

#endif