#version 330 core

layout(location = 0) in vec3 vert;

uniform float width;
uniform float height;

void main()
{
	float scaleFactor = 0.01f;
	vec4 ip = vec4(vert.xyz, 1.0f);

	float whr = width / height;
	float hwr = height / width;

	ip.x *= scaleFactor;
	ip.y *= scaleFactor * whr;
	ip.z *= scaleFactor;
	gl_Position = ip;
}
