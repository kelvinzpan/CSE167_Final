#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;
uniform vec4 clippingPlane;
uniform int noclip;

out vec3 surfaceNormal;
out vec3 baseNormal;
out vec3 vertex;

void main()
{
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0f);

	vec4 worldPos = model * vec4(position, 1.0f);
	gl_ClipDistance[0] = dot(worldPos, clippingPlane);

	surfaceNormal = mat3(transpose(inverse(modelview))) * normal;
	baseNormal = normal;
	vertex = vec3(model * vec4(position, 1.0f));
}