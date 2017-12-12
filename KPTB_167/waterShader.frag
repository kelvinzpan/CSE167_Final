#version 330 core
in vec4 clipSpace;
in vec2 textureCoords;
in vec3 toCamera;

out vec4 color;
uniform sampler2D textureTest;

uniform sampler2D reflection;
uniform sampler2D refraction;
uniform sampler2D dudvMap;
uniform float moveFactor;

const float modifier = 0.02;

void main(void) {

	vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0f + 0.5f;
	vec2 refractCoords = vec2(ndc.x, ndc.y);
	vec2 reflectCoords = vec2(ndc.x, -ndc.y);

	vec2 distortion1 = (texture(dudvMap, vec2(textureCoords.x + moveFactor, textureCoords.y)).rg * 2.0 - 1.0) * modifier;
	vec2 distortion2 = (texture(dudvMap, vec2(-textureCoords.x + moveFactor, textureCoords.y + moveFactor)).rg * 2.0 - 1.0) * modifier;
	vec2 totalDistort = distortion1 + distortion2;

	refractCoords += totalDistort;
	refractCoords = clamp(refractCoords, 0.001, 0.999);
	reflectCoords += totalDistort;
	reflectCoords.x = clamp(reflectCoords.x, 0.001, 0.999);
	reflectCoords.y = clamp(reflectCoords.y, -0.999, - 0.001);

	vec3 viewVec = normalize(toCamera);
	float refractiveFactor = dot(viewVec, vec3(0.0f, 1.0f, 0.0f));
	//change how reflective it is
	refractiveFactor = pow(refractiveFactor, 0.8f);	

	vec4 colorReflect = texture(reflection, reflectCoords);
	vec4 colorRefract = texture(refraction, refractCoords);

	color = mix(colorReflect, colorRefract, refractiveFactor);
	color = mix(color, vec4(0.0f, 0.3f, 0.5f, 1.0f), 0.2);
}