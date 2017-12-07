#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;

out vec3 baseNormal;
out vec3 vertex;
out vec3 baseColor;

void main()
{
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0f);
	baseNormal = normal;
	vertex = vec3(model * vec4(position, 1.0f));
	baseColor = color;
}