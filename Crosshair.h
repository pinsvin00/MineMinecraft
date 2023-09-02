#pragma once
#include "Shader.h"

class Crosshair {
public:
   Shader* shader = nullptr;

   static void init();
   static unsigned int VAO;
   static unsigned int VBO;

   void draw();

};