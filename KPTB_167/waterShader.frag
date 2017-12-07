#version 330 core
in vec2 textureCoords;

out vec4 color;
uniform sampler2D textureTest;
void main(void) {

	color = vec4(0.0, 0.0, 1.0, 1.0);
	vec4 texColor = texture(textureTest, textureCoords);
	color = texColor;
}