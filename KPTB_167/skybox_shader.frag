#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
	// Apply toon shading to skybox
	const float levels = 12.0f;
	const float brightness = 3.0f;
	vec4 texColor = texture(skybox, TexCoords);
	float amount = (texColor.r + texColor.g + texColor.b) / 3.0f;
	amount = floor(amount * levels) / levels;
	texColor.rgb = amount * vec3(texColor) * brightness;

    FragColor = texColor;
}