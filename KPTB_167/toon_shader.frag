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

in vec3 surfaceNormal;
in vec3 vertex;

out vec4 color;

void main()
{
	vec3 c_diff, c_spec, c_ambi;

	vec3 n = normalize(surfaceNormal);
	vec3 d = normalize(light.light_dir);
	vec3 L_dir = -d;
	vec3 c_l = light.light_color;

	c_diff = c_l * material.color_diff * dot(normalize(n), normalize(L_dir));

	vec3 R = normalize( 2 * dot(L_dir, n) * n - L_dir );
	vec3 eye_pos = vec3(0.0f, 0.0f, 20.0f);
	vec3 e = normalize(eye_pos - vertex);
	c_spec = c_l * material.color_spec * pow( dot( R, e ), material.spec_shine);

	c_ambi = c_l * material.color_ambi;
		
	vec3 matColor = c_diff + c_spec + c_ambi;
	color = vec4(matColor.x, matColor.y, matColor.z, 1.0);
}