#version 330 core

// Interpolated values from the vertex shaders
//in vec2 UV;
in vec4 particlecolor;
in vec2 textureCoords;
// Ouput data
out vec4 color;

uniform sampler2D texture;

void main(){
	// Output color = color of the texture at the specified UV

	//color = texture( texture, textureCoords );
	color = particlecolor;
}