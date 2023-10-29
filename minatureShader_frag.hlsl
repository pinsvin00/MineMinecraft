#version 330 core
out vec4 FragColor;
in vec2 TexCoord;


uniform sampler2D texture1;

void main()
{

   //ambient light
   vec4 full = texture(texture1, TexCoord).rgba;
   FragColor = full;
}