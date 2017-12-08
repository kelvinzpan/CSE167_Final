#include "Water.h"
#include "stb_image.h"
Water::Water()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind VAO to start binding rest of the buffers
	glBindVertexArray(VAO);

	// Bind VBO for vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// EBO for indices, so we know order to render
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Unbind current VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	initializeFrameBuffers();
	textureID = loadTexture("res/test_texture.jpg");
}

void Water::initializeFrameBuffers()
{
	//reflection fbo
	glGenFramebuffers(1, &reflectionFB);
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFB);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	//reflection texture
	glGenTextures(1, &reflectionTexture);
	glBindTexture(GL_TEXTURE_2D, reflectionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, REFLECTION_WIDTH, REFLECTION_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, reflectionTexture, 0);

	//depth render buffer
	glGenRenderbuffers(1, &reflectDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, reflectDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, REFLECTION_WIDTH, REFLECTION_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, reflectDepthBuffer);
	//unbind idk if necessary
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//refraction fbo
	glGenFramebuffers(1, &refractionFB);
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFB);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
		
	//refraction texture
	glGenTextures(1, &refractionTexture);
	glBindTexture(GL_TEXTURE_2D, refractionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, REFLECTION_WIDTH, REFLECTION_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, refractionTexture, 0);

	//depth buffer texture for refraction
	glGenTextures(1, &refractDepthTexture);
	glBindTexture(GL_TEXTURE_2D, refractDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, REFLECTION_WIDTH, REFLECTION_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, refractDepthTexture, 0);

	//unbind, this will go back to normal frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Water::draw(GLuint shader)
{
	shaderProgram = shader;
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &Window::V[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &Window::C[0][0]);
	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflectionTexture);
	glUniform1i(glGetUniformLocation(shaderProgram, "textureTest"), 0);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void Water::bindReflectionBuffer()
{
	glBindTexture(GL_TEXTURE_2D, 0);//To make sure the texture isn't bound
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFB);
	glViewport(0, 0, REFRACTION_WIDTH, REFRACTION_HEIGHT);
}

void Water::bindRefractionBuffer()
{
	glBindTexture(GL_TEXTURE_2D, 0);//To make sure the texture isn't bound
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFB);
	glViewport(0, 0, REFRACTION_WIDTH, REFRACTION_HEIGHT);
}

void Water::unbindBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::width, Window::height);
}

unsigned int Water::loadTexture(char * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	int width, height, nrChannels;

	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Cubemap texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
}

Water::~Water()
{

}
