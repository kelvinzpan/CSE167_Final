#include "Window.h"

// Window parameters
const char* window_title = "GLFW Starter Project";
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
double Window::mousePosX = 0.0;
double Window::mousePosY = 0.0;

// Default object shader
GLint shaderProgram;
#define VERTEX_SHADER_PATH "shader.vert"
#define FRAGMENT_SHADER_PATH "shader.frag"

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

void Window::initialize_objects()
{
	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);

	// Set up skybox
	Window::skybox = new Skybox();
	Window::skyboxShaderProgram = LoadShaders(SKY_VERTEX_SHADER_PATH, SKY_FRAGMENT_SHADER_PATH);
	glUseProgram(Window::skyboxShaderProgram);
	glUniform1i(glGetUniformLocation(Window::skyboxShaderProgram, "skybox"), 0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	glDeleteProgram(shaderProgram);

	glDeleteProgram(Window::skyboxShaderProgram);
	delete(Window::skybox);
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
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Skybox
	glUseProgram(Window::skyboxShaderProgram);
	Window::skybox->draw(Window::skyboxShaderProgram);

	// Use the shader of programID
	glUseProgram(shaderProgram);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	switch (action)
	{
	case GLFW_PRESS:
		glfwGetCursorPos(window, &mousePosX, &mousePosY);
		std::cout << "x: " << mousePosX << ", y: " << mousePosY << std::endl;
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "Mouse left-clicked." << std::endl;
			pressMouseLeft = true;
			break;

		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "Mouse right-clicked." << std::endl;
			pressMouseRight = true;
			break;
		}
		break;

	case GLFW_RELEASE:
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "Released mouse left-click." << std::endl;
			pressMouseLeft = false;
			break;

		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "Released mouse right-click." << std::endl;
			pressMouseRight = false;
			break;
		}
		break;
	}
}

void Window::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (pressMouseLeft)
	{
		// Trackball rotate by cursor movement
		glm::vec3 prevPos = trackballMap(glm::vec3(mousePosX, mousePosY, 0));
		glm::vec3 newPos = trackballMap(glm::vec3(xpos, ypos, 0));
		glm::vec3 direction = newPos - prevPos;
		float velocity = (float)direction.length();
		if (velocity > 0.001f) // Ignore small changes
		{
			// Rotate about the axis that is perpendicular to the great circle connecting the mouse movements
			glm::vec3 rotAxis = glm::vec4(glm::cross(prevPos, newPos), 0.0f) * V;
			float rotAngle = velocity * 0.03f;

			glm::mat4 rotateMat = glm::rotate(glm::mat4(1.0f), rotAngle, rotAxis);
			cam_pos = glm::vec3(glm::vec4(cam_pos, 1.0f) * rotateMat);
			V = glm::lookAt(cam_pos, cam_look_at, cam_up);
		}
	}
	if (pressMouseRight)
	{
		// Not necessary
	}
	if (pressMouseLeft || pressMouseRight)
	{
		mousePosX = xpos;
		mousePosY = ypos;
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
	}
	else if (yoffset < 0)
	{
		// Scrolling down
	}
}