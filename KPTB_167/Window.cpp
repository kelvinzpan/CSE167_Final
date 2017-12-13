#include "Window.h"

// NOTE: Model to world coordinates are: +X is LEFT, +Y is UP, +Z is IN

// Window parameters
const char* window_title = "Pest Control";
int Window::width;
int Window::height;
glm::mat4 Window::P;
glm::mat4 Window::V;

// Skybox parameters
#define SKY_VERTEX_SHADER_PATH "skybox_shader.vert"
#define SKY_FRAGMENT_SHADER_PATH "skybox_shader.frag"
Skybox* Window::skybox;
GLint Window::skyboxShaderProgram;

// Mouse control parameters
bool Window::pressMouseLeft = false;
bool Window::pressMouseRight = false;
bool Window::showParticleCount = false;
bool Window::noTerrain = false;
bool Window::noWater = false;
double Window::mousePosX = 0.0;
double Window::mousePosY = 0.0;

// Default object shader
GLint shaderProgram;
#define VERTEX_SHADER_PATH "shader.vert"
#define FRAGMENT_SHADER_PATH "shader.frag"

// Toon shader
GLint toonShaderProgram;
#define TOON_VERTEX_SHADER_PATH "toon_shader.vert"
#define TOON_FRAGMENT_SHADER_PATH "toon_shader.frag"

// Water shader
GLint waterShaderProgram;
#define WATER_VERTEX_SHADER_PATH "waterShader.vert"
#define WATER_FRAGMENT_SHADER_PATH "waterShader.frag"

// Particle shader
GLint particleShaderProgram;
#define PARTICLE_VERTEX_SHADER_PATH "particleShader.vert"
#define PARTICLE_FRAGMENT_SHADER_PATH "particleShader.frag"

// Setup camera objects for FPS view
Camera* Window::charCam;
Camera* Window::worldCam;
Camera* Window::currCam;
float Window::fpsYOffset;
bool Window::usingCharCam;
bool Window::initCamera = false;
float Window::horizSens;
float Window::vertSens;

// Movement parameters
float Window::playerSpeed = 0.3f;
float Window::playerSpeedNorm = 0.3f;
float Window::playerSpeedUp = 0.6f;
float Window::playerSpeedDown = 0.17f;
bool Window::playerSpeeding = false;
bool Window::pressingW = false;
bool Window::pressingA = false;
bool Window::pressingS = false;
bool Window::pressingD = false;
int keysPressed = 0;

// Scene Graph parameters
glm::mat4 Window::C;
MatrixTransform* world;

Group* player;
MatrixTransform* playerMT;
Geode* playerModel;

Group* test;
MatrixTransform* testMT;
Geode* testModel;

// Procedural terrain parameters
int Window::terrainSize = 500; // How detailed
int Window::terrainSeed = -1; // If -1, use default seed
GLint Window::terrainShaderProgram;
#define TERRAIN_VERTEX_SHADER_PATH "terrain_toon_shader.vert"
#define TERRAIN_FRAGMENT_SHADER_PATH "terrain_toon_shader.frag"

Terrain* Window::currTerrain;
Terrain* Window::baseTerrain;
Terrain* Window::randTerrain;
bool Window::showingRand = false;
int Window::randSeed;

// Particle effect parameters
ParticleSpawn * testSpawner;
Water * waterTest;

GLuint clippingPlaneLoc;

// Initialize all of our variables
void Window::initialize_objects()
{
	// Separate initialize camera because bugs
	Window::initializeCamera();

	// Load the shader program. Make sure you have the correct filepath up top
	toonShaderProgram = LoadShaders(TOON_VERTEX_SHADER_PATH, TOON_FRAGMENT_SHADER_PATH);
	waterShaderProgram = LoadShaders(WATER_VERTEX_SHADER_PATH, WATER_FRAGMENT_SHADER_PATH);
	particleShaderProgram = LoadShaders(PARTICLE_VERTEX_SHADER_PATH, PARTICLE_FRAGMENT_SHADER_PATH);

	// Set up skybox
	Window::skybox = new Skybox();
	Window::skyboxShaderProgram = LoadShaders(SKY_VERTEX_SHADER_PATH, SKY_FRAGMENT_SHADER_PATH);
	glUseProgram(Window::skyboxShaderProgram);
	glUniform1i(glGetUniformLocation(Window::skyboxShaderProgram, "skybox"), 0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Set up scene graph
	initialize_scene_graph();

	// Set up terrain
	Window::baseTerrain = new Terrain(Window::terrainSize, Window::terrainSeed);
	srand(time(0));
	Window::randSeed = rand();
	Window::randTerrain = new Terrain(Window::terrainSize, Window::randSeed);
	Window::currTerrain = Window::baseTerrain;
	terrainShaderProgram = LoadShaders(TERRAIN_VERTEX_SHADER_PATH, TERRAIN_FRAGMENT_SHADER_PATH);

	// Set up particle effects
	testSpawner = new ParticleSpawn();

	waterTest = new Water();

	clippingPlaneLoc = glGetUniformLocation(skyboxShaderProgram, "clippingPlane");
		
	std::cout << "Completed initialization of window objects." << std::endl;
}

void Window::initialize_scene_graph()
{
	Window::C = glm::mat4(1.0f);
	world = new MatrixTransform();

	player = new Group();
	world->addChild(player);
	playerMT = new MatrixTransform();
	player->addChild(playerMT);
	playerModel = new Geode("res/objects/cat.obj");
	playerMT->addChild(playerModel);
	playerModel->setParentMT(playerMT);
	playerModel->initSize(15.0f, false);
	playerModel->dontDraw = true; // We are initially in FPS mode

	// Add more models here
	test = new Group();
	world->addChild(test);
	testMT = new MatrixTransform();
	test->addChild(testMT);
	testModel = new Geode("res/objects/gull.obj");
	testMT->addChild(testModel);
	testModel->setParentMT(testMT);
	testModel->initSize(15.0f, false);
}

void Window::initializeCamera()
{
	if (!Window::initCamera)
	{
		// Set up camera on character view
		Window::charCam = new Camera();
		Window::fpsYOffset = 7.0f;
		Window::charCam->setPos(Window::charCam->cam_pos + glm::vec3(0.0f, Window::fpsYOffset, 0.0f));
		Window::charCam->setLookDir(glm::vec3(1.0f, 0.0f, 0.0f));

		Window::worldCam = new Camera(); // Manually set coords, don't use cam_look_dir
		Window::worldCam->cam_pos = glm::vec3(-40.0f, 40.0f, 0.0f);
		Window::worldCam->cam_look_at = glm::vec3(0.0f, 0.0f, 0.0f);

		Window::currCam = Window::charCam;
		V = glm::lookAt(Window::currCam->cam_pos, Window::currCam->cam_look_at, Window::currCam->cam_up);
		Window::usingCharCam = true;

		Window::horizSens = 0.05f;
		Window::vertSens = 0.025f;

		Window::initCamera = true;
	}
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	glDeleteProgram(shaderProgram);

	glDeleteProgram(Window::skyboxShaderProgram);
	delete(Window::skybox);

	delete(Window::baseTerrain);
	delete(Window::randTerrain);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 2000.0f);
		Window::initializeCamera();
		V = glm::lookAt(Window::currCam->cam_pos, Window::currCam->cam_look_at, Window::currCam->cam_up);
	}
}

void Window::idle_callback()
{
	Window::handleMovement();
	world->update();
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CLIP_DISTANCE0);

	renderSceneClippingReflect();	
	renderSceneClippingRefract(); 	

	renderScene();

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::renderSceneClippingReflect()
{
	waterTest->bindReflectionBuffer();
	float distance = 2 * (Window::currCam->cam_pos.y - waterTest->waterHeight);
	//tutorial uses y, but z looks better/accurate
	Window::currCam->cam_pos.y -= distance;
	V = glm::lookAt(Window::currCam->cam_pos, Window::currCam->cam_look_at, Window::currCam->cam_up);

	glm::vec4 plane = glm::vec4(0.0f, 1.0f, 0.0f, -0.02f);
	glUniform4f(clippingPlaneLoc, plane.x, plane.y, plane.z, plane.w);
	glUseProgram(Window::skyboxShaderProgram);
	Window::skybox->draw(Window::skyboxShaderProgram);

	waterTest->unbindBuffer();
	Window::currCam->cam_pos.y += distance;
	V = glm::lookAt(Window::currCam->cam_pos, Window::currCam->cam_look_at, Window::currCam->cam_up);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::renderSceneClippingRefract()
{
	waterTest->bindRefractionBuffer();

	glm::vec4 plane = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
	glUniform4f(clippingPlaneLoc, plane.x, plane.y, plane.z, plane.w);
	// Skybox (MUST DRAW LAST)
	glUseProgram(Window::skyboxShaderProgram);
	Window::skybox->draw(Window::skyboxShaderProgram);
	
	waterTest->unbindBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::renderScene()
{
	if (!noTerrain)
	{
		glUseProgram(terrainShaderProgram);
		glUniform4f(glGetUniformLocation(skyboxShaderProgram, "clippingPlane"), 0.0f, 0.0f, 0.0f, 0.0f);
		Window::currTerrain->draw(terrainShaderProgram, Window::C);
	}

	// Use the shader of programID
	glUseProgram(toonShaderProgram);
	glUniform4f(glGetUniformLocation(skyboxShaderProgram, "clippingPlane"), 0.0f, 0.0f, 0.0f, 0.0f);
	world->draw(toonShaderProgram, Window::C);

	glUseProgram(particleShaderProgram);
	testSpawner->draw(particleShaderProgram, Window::C);

	if (!noWater)
	{
		glDisable(GL_CLIP_DISTANCE0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(waterShaderProgram);
		waterTest->draw(waterShaderProgram, Window::C);

		glDisable(GL_BLEND);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Skybox (MUST DRAW LAST)
	glUseProgram(Window::skyboxShaderProgram);
	glUniform4f(clippingPlaneLoc, 0.0f, 1.0f, 0.0f, 10000000000.0f);
	Window::skybox->draw(Window::skyboxShaderProgram);
}

void Window::handleMovement()
{
	if (Window::usingCharCam)
	{
		float yThreshold = -2.0f;
		if (playerMT->newMat[3][1] < 0.0f)
			Window::playerSpeed = Window::playerSpeedDown;
		else
			if (Window::playerSpeeding)
				Window::playerSpeed = Window::playerSpeedUp;
			else
				Window::playerSpeed = Window::playerSpeedNorm;

		if (Window::pressingW && keysPressed <= 2)
		{
			glm::vec3 xz = glm::normalize(Window::currCam->cam_look_dir);
			float limit = Window::currTerrain->SIZE / 2.0f;
			if (playerMT->newMat[3][0] + xz.x <= -limit || playerMT->newMat[3][0] + xz.x >= limit) xz.x = 0;
			if (playerMT->newMat[3][2] + xz.z <= -limit || playerMT->newMat[3][2] + xz.z >= limit) xz.z = 0;

			glm::mat4 movement = glm::translate(glm::mat4(1.0f), glm::vec3(xz.x, 0, xz.z) * Window::playerSpeed);
			playerMT->translateOnce(movement);
			Window::currCam->setPos(glm::vec3(playerMT->newMat[3][0], playerMT->newMat[3][1], playerMT->newMat[3][2]));

			float y = Window::currTerrain->getRenderedHeight(Window::currCam->cam_pos.x, Window::currCam->cam_pos.z);
			if (y < yThreshold) y = yThreshold;
			playerMT->translateOnce(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, y - Window::currCam->cam_pos.y, 0.0f)));
			Window::currCam->setPos(glm::vec3(playerMT->newMat[3][0], playerMT->newMat[3][1] + Window::fpsYOffset, playerMT->newMat[3][2]));
			
			V = glm::lookAt(Window::currCam->cam_pos, Window::currCam->cam_look_at, Window::currCam->cam_up);
		}
		if (Window::pressingA && keysPressed <= 2)
		{
			glm::vec3 xz = glm::normalize(-glm::cross(Window::currCam->cam_look_dir, Window::currCam->cam_up));
			float limit = Window::currTerrain->SIZE / 2.0f;
			if (playerMT->newMat[3][0] + xz.x <= -limit || playerMT->newMat[3][0] + xz.x >= limit) xz.x = 0;
			if (playerMT->newMat[3][2] + xz.z <= -limit || playerMT->newMat[3][2] + xz.z >= limit) xz.z = 0;

			glm::mat4 movement = glm::translate(glm::mat4(1.0f), glm::vec3(xz.x, 0, xz.z) * Window::playerSpeed);
			playerMT->translateOnce(movement);
			Window::currCam->setPos(glm::vec3(playerMT->newMat[3][0], playerMT->newMat[3][1], playerMT->newMat[3][2]));

			float y = Window::currTerrain->getRenderedHeight(Window::currCam->cam_pos.x, Window::currCam->cam_pos.z);
			if (y < yThreshold) y = yThreshold;
			playerMT->translateOnce(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, y - Window::currCam->cam_pos.y, 0.0f)));
			Window::currCam->setPos(glm::vec3(playerMT->newMat[3][0], playerMT->newMat[3][1] + Window::fpsYOffset, playerMT->newMat[3][2]));

			V = glm::lookAt(Window::currCam->cam_pos, Window::currCam->cam_look_at, Window::currCam->cam_up);
		}
		if (Window::pressingS && keysPressed <= 2)
		{
			glm::vec3 xz = glm::normalize(-Window::currCam->cam_look_dir);
			float limit = Window::currTerrain->SIZE / 2.0f;
			if (playerMT->newMat[3][0] + xz.x <= -limit || playerMT->newMat[3][0] + xz.x >= limit) xz.x = 0;
			if (playerMT->newMat[3][2] + xz.z <= -limit || playerMT->newMat[3][2] + xz.z >= limit) xz.z = 0;

			glm::mat4 movement = glm::translate(glm::mat4(1.0f), glm::vec3(xz.x, 0, xz.z) * Window::playerSpeed);
			playerMT->translateOnce(movement);
			Window::currCam->setPos(glm::vec3(playerMT->newMat[3][0], playerMT->newMat[3][1], playerMT->newMat[3][2]));

			float y = Window::currTerrain->getRenderedHeight(Window::currCam->cam_pos.x, Window::currCam->cam_pos.z);
			if (y < yThreshold) y = yThreshold;
			playerMT->translateOnce(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, y - Window::currCam->cam_pos.y, 0.0f)));
			Window::currCam->setPos(glm::vec3(playerMT->newMat[3][0], playerMT->newMat[3][1] + Window::fpsYOffset, playerMT->newMat[3][2]));

			V = glm::lookAt(Window::currCam->cam_pos, Window::currCam->cam_look_at, Window::currCam->cam_up);
		}
		if (Window::pressingD && keysPressed <= 2)
		{
			glm::vec3 xz = glm::normalize(glm::cross(Window::currCam->cam_look_dir, Window::currCam->cam_up));
			float limit = Window::currTerrain->SIZE / 2.0f;
			if (playerMT->newMat[3][0] + xz.x <= -limit || playerMT->newMat[3][0] + xz.x >= limit) xz.x = 0;
			if (playerMT->newMat[3][2] + xz.z <= -limit || playerMT->newMat[3][2] + xz.z >= limit) xz.z = 0;

			glm::mat4 movement = glm::translate(glm::mat4(1.0f), glm::vec3(xz.x, 0, xz.z) * Window::playerSpeed);
			playerMT->translateOnce(movement);
			Window::currCam->setPos(glm::vec3(playerMT->newMat[3][0], playerMT->newMat[3][1], playerMT->newMat[3][2]));

			float y = Window::currTerrain->getRenderedHeight(Window::currCam->cam_pos.x, Window::currCam->cam_pos.z);
			if (y < yThreshold) y = yThreshold;
			playerMT->translateOnce(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, y - Window::currCam->cam_pos.y, 0.0f)));
			Window::currCam->setPos(glm::vec3(playerMT->newMat[3][0], playerMT->newMat[3][1] + Window::fpsYOffset, playerMT->newMat[3][2]));

			V = glm::lookAt(Window::currCam->cam_pos, Window::currCam->cam_look_at, Window::currCam->cam_up);
		}
	}
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	switch (action)
	{
	case GLFW_PRESS:

		switch (key)
		{
		// Close the window. This causes the program to also terminate.
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;

		// Toggle between base terrain and random terrain
		case GLFW_KEY_R:
			if (Window::showingRand)
			{
				Window::currTerrain = Window::baseTerrain;
				Window::showingRand = false;
			}
			else
			{
				Window::currTerrain = Window::randTerrain;
				Window::showingRand = true;
			}
			break;

		// Re-seed and re-generate the terrain.
		case GLFW_KEY_T:
			Window::randSeed = rand();
			delete(Window::randTerrain);
			Window::randTerrain = new Terrain(Window::terrainSize, Window::randSeed);
			Window::currTerrain = Window::randTerrain;
			break;

		// Alternate between textured terrain and flat shading.
		case GLFW_KEY_Y:
			Window::currTerrain->swapColors();
			break;

		// Toggle particle effects AND particle count
		case GLFW_KEY_P:
			Window::showParticleCount = !Window::showParticleCount;
			// TODO PARTICLE TOGGLE
			break;

		// Toggle rendering terrain
		case GLFW_KEY_N:
			Window::noTerrain = !Window::noTerrain;
			break;

		// Toggle rendering water
		case GLFW_KEY_L:
			Window::noWater = !Window::noWater;
			break;

		// Toggle FPS camera locked to character
		case GLFW_KEY_C:
			if (Window::usingCharCam)
			{
				Window::usingCharCam = false;
				Window::currCam = Window::worldCam;
				V = glm::lookAt(Window::currCam->cam_pos, Window::currCam->cam_look_at, Window::currCam->cam_up);

				playerModel->dontDraw = false;

				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			else
			{
				Window::usingCharCam = true;
				Window::currCam = Window::charCam;
				V = glm::lookAt(Window::currCam->cam_pos, Window::currCam->cam_look_at, Window::currCam->cam_up);

				playerModel->dontDraw = true;

				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			break;

		// WASD movement, W is forward
		case GLFW_KEY_W:
			Window::pressingW = true;
			keysPressed++;
			break;

		// A is left
		case GLFW_KEY_A:
			Window::pressingA = true;
			keysPressed++;
			break;

		// S is backwards
		case GLFW_KEY_S:
			Window::pressingS = true;
			keysPressed++;
			break;

		// D is right
		case GLFW_KEY_D:
			Window::pressingD = true;
			keysPressed++;
			break;

		// Press to increase speed
		case GLFW_KEY_LEFT_SHIFT:
			Window::playerSpeeding = true;
			break;
		}
		break; // End of GLFW_PRESS

	case GLFW_RELEASE:
		switch (key)
		{
		// WASD movement, W is forward
		case GLFW_KEY_W:
			Window::pressingW = false;
			keysPressed--; 
			break;

		// A is left
		case GLFW_KEY_A:
			Window::pressingA = false;
			keysPressed--;
			break;

		// S is backwards
		case GLFW_KEY_S:
			Window::pressingS = false;
			keysPressed--;
			break;

		// D is right
		case GLFW_KEY_D:
			Window::pressingD = false;
			keysPressed--;
			break;
		// Release to return to normal speed
		case GLFW_KEY_LEFT_SHIFT:
			Window::playerSpeeding = false;
			break;
		}
		break; // End of GLFW_RELEASE
	}
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	switch (action)
	{
	case GLFW_PRESS:
		glfwGetCursorPos(window, &Window::mousePosX, &Window::mousePosY);
		std::cout << "x: " << Window::mousePosX << ", y: " << Window::mousePosY << std::endl;
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "Mouse left-clicked." << std::endl;
			Window::pressMouseLeft = true;
			break;

		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "Mouse right-clicked." << std::endl;
			Window::pressMouseRight = true;
			break;

		}
		break;

	case GLFW_RELEASE:
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "Released mouse left-click." << std::endl;
			Window::pressMouseLeft = false;
			break;

		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "Released mouse right-click." << std::endl;
			Window::pressMouseRight = false;
			break;
		}
		break;
	}
}

void Window::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (Window::usingCharCam) // FPS camera when using char cam
	{
		// Change cam_look_dir based on mouse movement, use trackball rotate
		glm::vec3 prevPos = trackballMap(glm::vec3(Window::mousePosX, Window::mousePosY, 0));
		glm::vec3 newPos = trackballMap(glm::vec3(xpos, ypos, 0));
		glm::vec3 direction = newPos - prevPos;
		float velocity = (float)direction.length();
		if (velocity > 0.001f) // Ignore small changes
		{
			// Rotate about the axis that is perpendicular to the great circle connecting the mouse movements
			glm::vec3 rotAxis = glm::vec4(glm::cross(prevPos, newPos), 0.0f) * V;
			float rotAngle = velocity * 0.05f;

			glm::mat4 rotateMat = glm::rotate(glm::mat4(1.0f), rotAngle, rotAxis);
			glm::vec3 lookIncrement = glm::normalize(glm::vec3(glm::vec4(Window::currCam->cam_look_dir, 0.0f) * rotateMat) - Window::currCam->cam_look_dir);

			glm::vec3 newLookDir = glm::vec3(Window::currCam->cam_look_dir.x + lookIncrement.x * Window::horizSens,
				Window::currCam->cam_look_dir.y + lookIncrement.y * Window::vertSens,
				Window::currCam->cam_look_dir.z + lookIncrement.z * Window::horizSens);

			Window::currCam->setLookDir( newLookDir );
			V = glm::lookAt(Window::currCam->cam_pos, Window::currCam->cam_look_at, Window::currCam->cam_up);
		}
	}
	else if (!Window::usingCharCam && Window::pressMouseLeft) // Only trackball rotate using world cam
	{
		// Trackball rotate by cursor movement
		glm::vec3 prevPos = trackballMap(glm::vec3(Window::mousePosX, Window::mousePosY, 0));
		glm::vec3 newPos = trackballMap(glm::vec3(xpos, ypos, 0));
		glm::vec3 direction = newPos - prevPos;
		float velocity = (float)direction.length();
		if (velocity > 0.001f) // Ignore small changes
		{
			// Rotate about the axis that is perpendicular to the great circle connecting the mouse movements
			glm::vec3 rotAxis = glm::vec4(glm::cross(prevPos, newPos), 0.0f) * V;
			float rotAngle = velocity * 0.05f;

			glm::mat4 rotateMat = glm::rotate(glm::mat4(1.0f), rotAngle, rotAxis);
			Window::currCam->cam_pos = glm::vec3(glm::vec4(Window::currCam->cam_pos, 1.0f) * rotateMat);
			if (Window::currCam->cam_pos.y < 0.0f) Window::currCam->cam_pos.y = 0.0f;
			V = glm::lookAt(Window::currCam->cam_pos, Window::currCam->cam_look_at, Window::currCam->cam_up);
		}
	}

	Window::mousePosX = xpos;
	Window::mousePosY = ypos;
}

// Helper function used to rotate in cursor_pos_callback
glm::vec3 Window::trackballMap(glm::vec3 mouse)
{
	glm::vec3 v;
	float d;
	v.x = (2.0f * mouse.x - Window::width) / Window::width;
	v.y = (Window::height - 2.0f * mouse.y) / Window::height;
	v.z = 0.0f;
	d = (float)v.length();
	d = (d < 1.0f) ? d : 1.0f;
	v.z = sqrtf(1.001f - d*d);
	glm::normalize(v);
	return v;
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset > 0)
	{
		// Scrolling up

		if (!Window::usingCharCam) // Zoom only on world cam
		{
			std::cout << "Scrolling up, zooming in." << std::endl;
			glm::vec3 currDir = glm::normalize(Window::currCam->cam_pos) * 5.0f;
			if (Window::currCam->cam_pos.x - currDir.x == 0.0f && Window::currCam->cam_pos.y - currDir.y == 0.0f && Window::currCam->cam_pos.z - currDir.z == 0.0f)
			{
				return;
			}
			glm::vec3 new_cam_pos = Window::currCam->cam_pos - currDir;
			if (glm::dot(new_cam_pos, Window::currCam->cam_pos) == 1)
			{
				return;
			}
			Window::currCam->cam_pos = new_cam_pos;
			V = glm::lookAt(Window::currCam->cam_pos, Window::currCam->cam_look_at, Window::currCam->cam_up);
		}
	}
	else if (yoffset < 0)
	{
		// Scrolling down

		if (!Window::usingCharCam) // Zoom only on world cam
		{
			std::cout << "Scrolling down, zooming out." << std::endl;
			glm::vec3 currDir = glm::normalize(Window::currCam->cam_pos) * 5.0f;
			float newX = Window::currCam->cam_pos.x + currDir.x;
			float newY = Window::currCam->cam_pos.y + currDir.y;
			float newZ = Window::currCam->cam_pos.z + currDir.z;
			Window::currCam->cam_pos = Window::currCam->cam_pos + currDir;
			V = glm::lookAt(Window::currCam->cam_pos, Window::currCam->cam_look_at, Window::currCam->cam_up);
		}
	}
}