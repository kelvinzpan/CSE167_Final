#version 330 core

struct Light
{
	vec3 light_color;
	vec3 light_dir;
};

uniform Light light;
uniform vec3 camPos;

in vec3 baseNormal;
in vec3 vertex;
in vec3 baseColor;

out vec4 color;

void main()
{
	vec3 c_ambi = baseColor;
	vec3 c_diff = baseColor;
	vec3 c_spec = baseColor;
	float spec_shine = 0.1f;

	vec3 c_l = light.light_color;
	vec3 N = normalize(baseNormal);
	vec3 L = -normalize(light.light_dir);
	vec3 E = normalize(camPos - vertex);

	// Silhouette edges
	float edge = max(0.0f, dot(N, E));
	if (false) // Turned off silhouettes. Use edge < 0.01f to turn on.
	{
		color = vec4(0, 0, 0, 1);
	}
	else
	{
		const float toonA = 0.1;
		const float toonB = 0.2;
		const float toonC = 0.3;
		const float toonD = 0.6;
		const float toonE = 0.9;
		const float toonF = 1.0;

		float diff = max(0.0f, dot(N, L));
		if (diff < toonA) diff = 0.0;
		else if (diff < toonB) diff = toonB;
		else if (diff < toonC) diff = toonC;
		else if (diff < toonD) diff = toonD;
		else if (diff < toonE) diff = toonE;
		else diff = toonF;

		vec3 R = normalize( 2 * dot(L, N) * N - L );
		float spec = pow( max(0.0f, dot( R, E )), spec_shine );
		spec = step(0.5f, spec);
		
		vec3 matColor = c_l * (c_diff * diff + c_spec * spec + c_ambi);
		color = vec4(matColor, 1.0);
	}
}