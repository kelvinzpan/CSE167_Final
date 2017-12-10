#version 330 core

layout (location = 0) in vec3 position;

out vec4 clipSpace;
out vec2 textureCoords;
out vec3 toCamera;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 modelview;
uniform vec3 cameraPos;

const float tiling = 6.0f;

void main(void) {
	vec4 worldPos = model * vec4(position.x, 0.0, position.y, 1.0);
	clipSpace = projection * view * worldPos;
	gl_Position = clipSpace;
	textureCoords = vec2(position.x/2000 + 0.5, position.z/2000 + 0.5) * tiling;
	toCamera = cameraPos - worldPos.xyz;
}