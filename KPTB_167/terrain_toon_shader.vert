#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 texCoords;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;
uniform vec4 clippingPlane;

out vec3 baseNormal;
out vec3 vertex;
out vec3 baseColor;
out vec2 pass_texCoords;

void main()
{
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0f);

	vec4 worldPos = model * vec4(position, 1.0f);
	gl_ClipDistance[0] = dot(worldPos, clippingPlane);

	baseNormal = normal;
	vertex = vec3(model * vec4(position, 1.0f));
	baseColor = color;
	pass_texCoords = texCoords;
}