#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec4 pos; // Position of the center of the particule and size of the square
layout(location = 2) in vec4 colors; // Position of the center of the particule and size of the square

// Output data ; will be interpolated for each fragment.
//out vec2 UV;
out vec4 particlecolor;

// Values that stay constant for the whole mesh.
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	float particleSize = 100.0f; // because we encoded it this way.
	vec3 particleCenter_wordspace = pos.xyz;
	
	//replaces the model portion of MVP
	vec3 vertexPosition_worldspace = 
		particleCenter_wordspace
		+ CameraRight_worldspace * squareVertices.x * particleSize
		+ CameraUp_worldspace * squareVertices.y * particleSize;

	// Output position of the vertex
	gl_Position = projection * view * (squareVertices, 1.0f) * vec4(vertexPosition_worldspace, 1.0f);

	// UV of the vertex. No special space for this one.
	//UV = squareVertices.xy + vec2(0.5, 0.5);
	particlecolor = colors;
}