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

// Default camera parameters
glm::vec3 Window::cam_pos;
glm::vec3 Window::cam_look_at;
glm::vec3 Window::cam_up;

// Scene Graph parameters
glm::mat4 Window::C;
MatrixTransform* world;

Group* player;
MatrixTransform* playerMT;
Geode* playerModel;

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
	// Set up camera
	Window::cam_pos = glm::vec3(0.0f, 0.0f, -40.0f);
	Window::cam_look_at = glm::vec3(0.0f, 0.0f, 0.0f);
	Window::cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
	V = glm::lookAt(Window::cam_pos, Window::cam_look_at, Window::cam_up);

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
	// testSpawner = new ParticleSpawn();

	std::cout << "Completed initialization of window objects." << std::endl;
	waterTest = new Water();
	testSpawner = new ParticleSpawn();

	clippingPlaneLoc = glGetUniformLocation(skyboxShaderProgram, "clippingPlane");
		
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

	glm::mat4 test = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 4.0f, 0.0f));
	playerMT->translateOnce(test);
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
		V = glm::lookAt(Window::cam_pos, Window::cam_look_at, Window::cam_up);
	}
}

void Window::idle_callback()
{
	world->update();
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CLIP_DISTANCE0);

	waterTest->bindReflectionBuffer();
	float distance = 2 * (cam_pos.y - waterTest->waterHeight);
	//tutorial uses y, but z looks better/accurate
	cam_pos.y -= distance;
	invertPitch();
	renderSceneClippingReflect();	//0 is reflect, 1 is refract
	waterTest->unbindBuffer();
	cam_pos.y += distance;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	invertPitch();

	waterTest->bindRefractionBuffer();
	renderSceneClippingRefract(); 	//0 is reflect, 1 is refract
	waterTest->unbindBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glDisable(GL_CLIP_DISTANCE0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(waterShaderProgram);
	waterTest->draw(waterShaderProgram, Window::C);
	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderScene();

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::invertPitch()
{
	float pitch = glm::asin(cam_look_at.y);
	float yaw = glm::acos(cam_look_at.x / glm::cos(pitch));
	float invertPitch = -pitch;

	cam_look_at.x = glm::cos(invertPitch)*glm::cos(yaw);
	cam_look_at.y = glm::sin(invertPitch);
	cam_look_at.z = glm::cos(invertPitch)*glm::sin(yaw);
}

void Window::renderSceneClippingReflect()
{
	glm::vec4 plane = glm::vec4(0.0f, 1.0f, 0.0f, -0.0f);
	glUniform4f(clippingPlaneLoc, plane.x, plane.y, plane.z, plane.w);
	// Skybox (MUST DRAW LAST)
	glUseProgram(Window::skyboxShaderProgram);
	Window::skybox->draw(Window::skyboxShaderProgram);
	
}

void Window::renderSceneClippingRefract()
{
	glm::vec4 plane = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
	glUniform4f(clippingPlaneLoc, plane.x, plane.y, plane.z, plane.w);
	// Skybox (MUST DRAW LAST)
	glUseProgram(Window::skyboxShaderProgram);
	Window::skybox->draw(Window::skyboxShaderProgram);
	
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

	// Skybox (MUST DRAW LAST)
	glUseProgram(Window::skyboxShaderProgram);
	glUniform4f(clippingPlaneLoc, 0.0f, 0.0f, 0.0f, 10000000000.0f);
	Window::skybox->draw(Window::skyboxShaderProgram);

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

		// Toggle particle count
		case GLFW_KEY_P:
			Window::showParticleCount = !Window::showParticleCount;
			break;

		// Toggle rendering terrain
		case GLFW_KEY_N:
			Window::noTerrain = !Window::noTerrain;
			break;
		}
		break; // End of GLFW_PRESS
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
	if (Window::pressMouseLeft)
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
			float rotAngle = velocity * 0.03f;

			glm::mat4 rotateMat = glm::rotate(glm::mat4(1.0f), rotAngle, rotAxis);
			Window::cam_pos = glm::vec3(glm::vec4(Window::cam_pos, 1.0f) * rotateMat);
			V = glm::lookAt(Window::cam_pos, Window::cam_look_at, Window::cam_up);
		}
	}
	if (Window::pressMouseRight)
	{
		// Not necessary
	}
	if (Window::pressMouseLeft || Window::pressMouseRight)
	{
		Window::mousePosX = xpos;
		Window::mousePosY = ypos;
	}
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
		std::cout << "Scrolling up, zooming in." << std::endl;
		glm::vec3 currDir = glm::normalize(Window::cam_pos) * 5.0f;
		if (Window::cam_pos.x - currDir.x == 0.0f && Window::cam_pos.y - currDir.y == 0.0f && Window::cam_pos.z - currDir.z == 0.0f)
		{
			return;
		}
		glm::vec3 new_cam_pos = Window::cam_pos - currDir;
		if (glm::dot(new_cam_pos, Window::cam_pos) == 1)
		{
			return;
		}
		Window::cam_pos = new_cam_pos;
		V = glm::lookAt(Window::cam_pos, Window::cam_look_at, Window::cam_up);
	}
	else if (yoffset < 0)
	{
		// Scrolling down
		std::cout << "Scrolling down, zooming out." << std::endl;
		glm::vec3 currDir = glm::normalize(Window::cam_pos) * 5.0f;
		float newX = Window::cam_pos.x + currDir.x;
		float newY = Window::cam_pos.y + currDir.y;
		float newZ = Window::cam_pos.z + currDir.z;
		Window::cam_pos = Window::cam_pos + currDir;
		V = glm::lookAt(Window::cam_pos, Window::cam_look_at, Window::cam_up);
	}
}