#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 aNormal;

out vec2 TexCoord;
uniform mat4 model;
uniform mat4 projection;
uniform int blockType;

void main()
{
   gl_Position = projection * model * vec4(aPos, 1.0f);
   TexCoord = vec2(texCoord.x, texCoord.y + (blockType/10.0f));
}