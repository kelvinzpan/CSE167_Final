#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 clippingPlane;
uniform int noclip;

void main()
{
    TexCoords = aPos;

	vec4 worldPos = model * vec4(aPos, 1.0f);
	gl_ClipDistance[0] = dot(worldPos, clippingPlane);

    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}  