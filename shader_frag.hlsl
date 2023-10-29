#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
flat in int instanceID;

struct SunLight {
   vec3 direction;
   vec3 ambient;
   vec3 diffuse;
};

uniform sampler2D texture1;
uniform SunLight light;
uniform int outlinedCubeIdx;

const float OUTLINE_X_OFFSET = 0.6;

vec2 localizeTextureCoords(vec2 arg)
{
   arg *= 10;
   vec2 res = vec2(arg.x - floor(arg.x), arg.y - floor(arg.y));
   res /= 10;
   return res;
}

void main()
{
   if (outlinedCubeIdx == instanceID)
   {
      vec2 outlineCorespondingCoords = localizeTextureCoords(TexCoord);
      outlineCorespondingCoords.x += OUTLINE_X_OFFSET;
      vec4 outlineColor = texture(texture1, outlineCorespondingCoords);
      if (outlineColor.w != 0.0)
      {
         FragColor = outlineColor;
         return;
      }

   }

   //ambient light
   vec4 full = texture(texture1, TexCoord).rgba;

   vec3 tex = full.xyz;
   float alpha = full.w;

   vec3 ambient = light.ambient * tex;
   //diffuse light
   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(light.direction);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = light.diffuse * diff * tex;

   vec3 result = ambient + diffuse;

   FragColor = vec4(result, alpha);
}