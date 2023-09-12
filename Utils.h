#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "Cube.h"
#include "PerlinNoise.h"

struct VBOPool {

   std::vector<unsigned int> vbos;
   size_t siz = 0;
   size_t top = 0;
   VBOPool() = default;
   void clear()
   {
      this->top = 0;
   }
   unsigned int get()
   {
      unsigned int vbo = vbos[top];
      top++;

      return vbo;
   }
   void init(size_t total)
   {
      this->siz = total;
      vbos.reserve(total);
      for (size_t i = 0; i < siz; i++)
      {
         unsigned int vbo = 0;
         glGenBuffers(1, &vbo);
         vbos.push_back(vbo);
      }
   }

};

struct CubeGPUStruct {
   glm::vec3 position;
   float blockType;
};

