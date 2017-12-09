#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;

// Output data ; will be interpolated for each fragment.
//out vec2 UV;
out vec4 particlecolor;
out vec2 textureCoords;

// Values that stay constant for the whole mesh.
//uniform mat4 model;
//uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelview;
uniform vec3 instancePos;
uniform vec4 instanceColor;

void main()
{
	vec4 particlePos = vec4(position.x + instancePos.x, position.y + instancePos.y, position.z + instancePos.z, 1.0f);
	// Output position of the vertex
	gl_Position = projection * modelview * particlePos;

	textureCoords = vec2(position.x/2.0 + 0.5, position.y/2.0 + 0.5);
	// UV of the vertex. No special space for this one.
	particlecolor = instanceColor;
}