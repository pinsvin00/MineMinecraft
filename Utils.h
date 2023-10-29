#ifndef UTILS_H
#define UTILS_H

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

struct KeyboardData {
private:
public:
   std::bitset<16> pressedKeys;
   static constexpr inline uint8_t KEY_W = 0;
   static constexpr inline uint8_t KEY_S = 1;
   static constexpr inline uint8_t KEY_A = 2;
   static constexpr inline uint8_t KEY_D = 3;
   static constexpr inline uint8_t KEY_SPACE = 4;

   bool isLocked = false;

   KeyboardData()
   {
      pressedKeys = 0;
   }

   void setKeyPressed(uint8_t key, bool isPressed)
   {
      pressedKeys[key] = isPressed;
   }

   void reset()
   {
      pressedKeys.reset();
   }

   bool isKeyPressed(uint8_t key)
   {
      return pressedKeys[key];
   }

};

#endif