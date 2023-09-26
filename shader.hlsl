#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aOffset;
layout(location = 4) in float blockType;

out vec2 TexCoord;
out vec3 Normal;
flat out int instanceID;

uniform mat4 view;
uniform mat4 projection;


void main()
{
   gl_Position = projection * view * vec4(aPos + aOffset, 1.0f);
   TexCoord = vec2(texCoord.x, texCoord.y + blockType);
   Normal = aNormal;
   instanceID = gl_InstanceID;
}