#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
uniform samplerCube skyboxSunMoon;
uniform samplerCube skyboxStars;
uniform float starsLighting;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


void main()
{
   vec4 transformedSunMoon = model * vec4(TexCoords, 1.0);
   vec4 sunMoonTex = texture(skyboxSunMoon, transformedSunMoon.xyz).rgba;
   vec4 starsTex = texture(skyboxStars, TexCoords).rgba;


   vec4 final = sunMoonTex;
   if (sunMoonTex.xyz == vec3(0.0))
   {
      final = starsTex;
      final.a *= starsLighting;
   }

   FragColor = final;
   return;
}