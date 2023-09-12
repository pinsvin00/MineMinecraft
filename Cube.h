#pragma once
#include "Shader.h"
#include "m_math.h"
#include <glm/gtx/transform.hpp>
#include <vector>
#include <bitset>
#include <array>
#include <iterator>


//enum Block {
//   GRASS = 0.0f, DIRT = 0.1, STONE, AIR
//};

const float GRASS    = 0.0f;
const float DIRT     = 0.1f;
const float STONE    = 0.2f;
const float WOOD     = 0.3f;
const float LEAVES   = 0.4f;
const float WATER    = 0.5f;


class Chunk;
class Cube {

private:
   Shader* cubeShader = nullptr;
public:
   //instance data of cube
   glm::mat4 model = glm::mat4(1.0f);
   glm::vec3 position = glm::vec3(0.0f);
   glm::vec3 chunkPosition = glm::vec3(0.0f);
   glm::vec3 scale = glm::vec3(1.0f);

   Shader* shader = nullptr;
   Chunk* cubesChunk = nullptr;
   bool destroyed = false;
   float blockKind = GRASS;
   uint8_t facesToRender = 0b111111;

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
   Cube(glm::vec3 position, float block);
   Cube() = default;

   template<typename T>
   bool checkCollision(T c);

   Box3 getCollider();

   std::array<glm::vec3,8> getVertices();
   void clearVertices();

   void processMat();
   void draw();
   static void init();
   static unsigned int VAO;
   static unsigned int VBO;
   static unsigned int EBO;

};


class RectangularCollider
{
    glm::mat4 model = glm::mat4(1.0f);


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

public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 origin = glm::vec3(0.0f);

    void calculateModel()
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, origin);
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
    }

    std::array<glm::vec3, 8> getVertices()
    {
        std::array<glm::vec3, 8> calculatedVerts;
        
        for (size_t i = 0; i < 8; i++)
        {
            glm::vec4 temp = model * glm::vec4(cubeVerts[i], 1.0f);
            calculatedVerts[i] = glm::vec3(temp.x, temp.y, temp.z);
        }

        return calculatedVerts;
    }

};

template<typename T>
bool Cube::checkCollision(T c)
{
    std::array<glm::vec3, 8> myVerts = this->getVertices();
    std::array<glm::vec3, 8> theirVerts = c->getVertices();

    auto m_vecMin_1 = myVerts[LEFT_DOWN_1];
    auto m_vecMax_1 = myVerts[RIGHT_UP_2];
    auto m_vecMin_2 = theirVerts[LEFT_DOWN_1];
    auto m_vecMax_2 = theirVerts[RIGHT_UP_2];


    return(
        m_vecMax_1.x > m_vecMin_2.x &&
        m_vecMin_1.x < m_vecMax_2.x &&
        m_vecMax_1.y > m_vecMin_2.y &&
        m_vecMin_1.y < m_vecMax_2.y &&
        m_vecMax_1.z > m_vecMin_2.z &&
        m_vecMin_1.z < m_vecMax_2.z
        );
}
