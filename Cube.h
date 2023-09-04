#pragma once
#include "Shader.h"
#include "m_math.h"
#include <glm/gtx/transform.hpp>
#include <vector>
#include <bitset>
#include <array>
#include <iterator>


enum Block {
   DIRT, STONE, AIR
};


class Cube {

private:
   Shader* cubeShader = nullptr;
public:
   //instance data of cube
   glm::mat4 model = glm::mat4(1.0f);
   glm::vec3 position = glm::vec3(0.0f);
   glm::vec3 idx = glm::vec3(0.0f);
   glm::vec3 scale = glm::vec3(1.0f);

   Shader* shader = nullptr;
   bool dontDraw = false;
   Block blockKind = Block::AIR;
   uint8_t facesToRender = 0;

   bool isInitialized = false;
   bool vertsCalculated = false;
   std::array<glm::vec3, 8> calculatedVerts = {
       glm::vec3(-0.5f, -0.5f, -0.5f), // left down
       glm::vec3(0.5f, -0.5f, -0.5f),  // right down
       glm::vec3(0.5f, 0.5f, -0.5f),  //right up
       glm::vec3(-0.5f, 0.5f, -0.5f), //left up

       glm::vec3(-0.5f, -0.5f, 0.5f), // left down
       glm::vec3(0.5f, -0.5f, 0.5f),  // right down
       glm::vec3(0.5f, 0.5f, 0.5f),  //right up
       glm::vec3(-0.5f, 0.5f, 0.5f), //left up
   };


   //static data of Cube
public:
   enum VERT_IDXS {
      LEFT_DOWN_1,
      RIGHT_DOWN_1,
      RIGHT_UP_1,
      LEFT_UP_1,

      LEFT_DOWN_2,
      RIGHT_DOWN_2,
      RIGHT_UP_2,
      LEFT_UP_2,
   };

   static constexpr inline std::array<glm::vec3, 8> cubeVerts = {
      glm::vec3(-0.5f, -0.5f, -0.5f), // left down
      glm::vec3(0.5f, -0.5f, -0.5f),  // right down
      glm::vec3(0.5f, 0.5f, -0.5f),  //right up
      glm::vec3(-0.5f, 0.5f, -0.5f), //left up

      glm::vec3(-0.5f, -0.5f, 0.5f), // left down
      glm::vec3(0.5f, -0.5f, 0.5f),  // right down
      glm::vec3(0.5f, 0.5f, 0.5f),  //right up
      glm::vec3(-0.5f, 0.5f, 0.5f), //left up
   };




   void setPosition(glm::vec3 position);
   Cube(glm::vec3 position, Block block);
   Cube() = default;
   bool checkCollision(Cube* c);
   Box3 getCollider();
   std::array<glm::vec3,8> getVertices();
   void processMat();
   void draw();
   static void init();
   static unsigned int VAO;
   static unsigned int VBO;
   static unsigned int EBO;

};


