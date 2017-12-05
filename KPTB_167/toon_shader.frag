#version 330 core

struct Material
{
	vec3 color_diff;
	vec3 color_spec;
	vec3 color_ambi;
	float spec_shine;
};

struct Light
{
	vec3 light_color;
	vec3 light_dir;
};

uniform Material material;
uniform Light light;
uniform vec3 camPos;

in vec3 surfaceNormal;
in vec3 vertex;

out vec4 color;

void main()
{
	vec3 c_diff, c_spec, c_ambi;

	// Silhouette edges
	float edge = max(0, dot(normalize(surfaceNormal), normalize(vertex - camPos)));
	if (edge < 0.01f)
	{
		color = vec4(0, 0, 0, 1);
	}
	else
	{
		vec3 n = normalize(surfaceNormal);
		vec3 d = normalize(light.light_dir);
		vec3 L_dir = -d;
		vec3 c_l = light.light_color;

		float diff = max(0, dot(n, L_dir));
		// Discrete brightness levels for toon shading
		const float levels = 3.0f;
		diff = floor(diff * levels) / levels;
		c_diff = c_l * material.color_diff * diff;

		vec3 R = normalize( 2 * dot(L_dir, n) * n - L_dir );
		vec3 e = normalize(camPos - vertex);
		float spec = max(0, pow( dot( R, e ), material.spec_shine ) );
		// Discrete shinyness levels for toon shading
		spec = floor(spec * levels) / levels;
		c_spec = c_l * material.color_spec * spec;

		c_ambi = c_l * material.color_ambi;
		
		vec3 matColor = c_diff + c_spec + c_ambi;
		color = vec4(matColor.x, matColor.y, matColor.z, 1.0);
	}
}