#version 330 core

layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 texCoord;

uniform mat4 model;
uniform mat4 proj;
uniform vec2 offset;
out vec2 TexCoord;

float lerp(float a, float b, float t)
{
	return a * (1 - t) + b * t;
}

void main()
{
	vec4 res = model * vec4(vert.xy, 1.0, 1.0);

	res.x = 2 * (res.x/1920) - 1;
	res.y = 2 * (res.y/1080) - 1;


	gl_Position = res;
	TexCoord = texCoord.xy;
}