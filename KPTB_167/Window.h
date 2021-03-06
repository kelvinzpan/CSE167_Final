#ifndef _WINDOW_H_
#define _WINDOW_H_

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

#include <iostream>
#include <time.h>

#include "shader.h"
#include "Skybox.h"
#include "Node.h"
#include "Group.h"
#include "Geode.h"
#include "MatrixTransform.h"
#include "ParticleSpawn.h"
#include "Terrain.h"
#include "Water.h"
#include "Camera.hpp"
#include "GullSpawner.hpp"

class Window
{
public:
	static int width;
	static int height;
	static glm::mat4 P; // P for projection
	static glm::mat4 V; // V for view
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static bool pressMouseLeft;
	static bool pressMouseRight;
	static bool showParticleCount;
	static bool show3DWater;
	static bool noTerrain;
	static bool noWater;
	static double mousePosX;
	static double mousePosY;
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static glm::vec3 trackballMap(glm::vec3 mouse);
	static void renderScene();
	static void renderSceneClippingReflect();
	static void renderSceneClippingRefract();

	static Skybox* skybox;
	static GLint skyboxShaderProgram;

	static void initialize_scene_graph();
	static glm::mat4 C;

	static Camera* currCam;
	static Camera* charCam;
	static Camera* worldCam;
	static bool usingCharCam;
	static float fpsYOffset;
	static bool initCamera;
	static void initializeCamera();
	static float horizSens;
	static float vertSens;

	static float playerSpeed;
	static float playerSpeedNorm;
	static float playerSpeedUp;
	static float playerSpeedDown;
	static bool playerSpeeding;
	static void handleMovement();
	static bool pressingW, pressingA, pressingS, pressingD;

	static GullSpawner* gullSpawner;
	static int gullCount;
	static float gullHeight;
	static float gullSpeed;

	static int currScore;
	static int highScore;
	static bool playerDamaged;
	static int currDamageFrames;
	static int maxDamageFrames;
	static int currInvinFrames;
	static int maxInvinFrames;
	static bool playerInvin;
	static void damagePlayer();
	static float hurtDist;

	static Terrain* currTerrain;
	static Terrain* baseTerrain;
	static Terrain* randTerrain;
	static bool showingRand;
	static int randSeed;
	static GLint terrainShaderProgram;
	static int terrainSize;
	static int terrainSeed;
	static GLint particleShaderProgram;
};

#endif
