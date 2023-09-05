#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 aOffset;

out vec2 TexCoord;
uniform mat4 view;
uniform mat4 projection;


void main()
{
   gl_Position = projection * view * vec4(aPos + aOffset, 1.0f);
   TexCoord = vec2(texCoord.x, texCoord.y);
}