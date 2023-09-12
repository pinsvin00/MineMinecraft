#include "Chunk.h"

void Chunk::addCube(Cube cube, size_t x, size_t y, size_t z)
{
   this->isCubeDataValid = false;
   this->cubesData[cubesCount] = std::move(cube);
   this->cubesData[cubesCount].cubesChunk = this;
   this->cubesMap[x][y][z] = &cubesData[cubesCount];
   this->cubesMap[x][y][z]->chunkPosition = glm::vec3(x, y, z);
   this->cubesCount += 1;

   this->isCubeDataValid = false;
}

std::optional<Cube*> Chunk::getCubeByChunkPos(int x, int y, int z)
{
   if (x < 0 || x >= CHUNK_SIZE_X)
   {
      return std::nullopt;
   }
   if (z < 0 || z >= CHUNK_SIZE_Z)
   {
      return std::nullopt;
   }
   if (y < 0 || y >= CHUNK_SIZE_Y)
   {
      return std::nullopt;
   }
   if (cubesMap[x][y][z] == nullptr)
   {
      return std::nullopt;
   }

   return cubesMap[x][y][z];
}

std::vector<Cube*> Chunk::getNeighboringCubes(Cube& cb)
{
   std::vector<Cube*> result;
   std::vector<glm::vec3> dirs = {
      glm::vec3(0,0,-1),
      glm::vec3(0,0 ,1),
      glm::vec3(-1,0,0),
      glm::vec3(1,0,0),
      glm::vec3(0,1,0),
      glm::vec3(0,1,0),
   };

   for (auto& direction : dirs)
   {
      glm::vec3 neighborPositon = cb.chunkPosition + direction;
      auto neighborCube = getCubeByChunkPos(neighborPositon.x, neighborPositon.y, neighborPositon.z);
      if (neighborCube.has_value())
      {
         result.emplace_back(neighborCube.value());
      }
   }


   return result;
}

void Chunk::calculateFace(Cube& cb)
{
   auto idx = cb.chunkPosition;
   std::vector<glm::vec3> dirs = {
      glm::vec3(0,0,-1),
      glm::vec3(0,0 ,1),
      glm::vec3(-1,0,0),
      glm::vec3(1,0,0),
      glm::vec3(0,1,0),
      glm::vec3(0,1,0),
   };


   size_t iter = 0;
   cb.facesToRender = 0;
   for (auto& element : dirs)
   {
      glm::vec3 pos = idx + element;
      auto cube = getCubeByChunkPos(
         (int)pos.x,
         (int)pos.y,
         (int)pos.z
      );

      if (cube.has_value() == false || cube.value()->destroyed == true)
      {
         cb.facesToRender |= 1 << iter;
      }
      iter++;
   }
}

void Chunk::calculateFaces()
{
   for (size_t i = 0; i < cubesCount; i++) {
      calculateFace(cubesData[i]);
   }
}


void Chunk::prepareGPU()
{

   const float cubeVertices[] = {
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, //back 
      0.5f, -0.5f, -0.5f,  0.1f, 0.0f,
      0.5f,  0.5f, -0.5f,  0.1f, 0.1f,
      0.5f,  0.5f, -0.5f,  0.1f, 0.1f,
      -0.5f,  0.5f, -0.5f,  0.0f, 0.1f,
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,


      -0.5f, -0.5f,  0.5f,  0.1f, 0.0f, //front
      0.5f, -0.5f,  0.5f,  0.2f, 0.0f,
      0.5f,  0.5f,  0.5f,  0.2f, 0.1f,
      0.5f,  0.5f,  0.5f,  0.2f, 0.1f,
      -0.5f,  0.5f,  0.5f,  0.1f, 0.1f,
      -0.5f, -0.5f,  0.5f,  0.1f, 0.0f,

      -0.5f,  0.5f,  0.5f,  0.3f, 0.0f, //left
      -0.5f,  0.5f, -0.5f,  0.3f, 0.1f,
      -0.5f, -0.5f, -0.5f,  0.2f, 0.1f,
      -0.5f, -0.5f, -0.5f,  0.2f, 0.1f,
      -0.5f, -0.5f,  0.5f,  0.2f, 0.0f,
      -0.5f,  0.5f,  0.5f,  0.3f, 0.0f,

      0.5f,  0.5f,  0.5f,  0.4f, 0.0f, //right
      0.5f,  0.5f, -0.5f,  0.4f, 0.1f,
      0.5f, -0.5f, -0.5f,  0.3f, 0.1f,
      0.5f, -0.5f, -0.5f,  0.3f, 0.1f,
      0.5f, -0.5f,  0.5f,  0.3f, 0.0f,
      0.5f,  0.5f,  0.5f,  0.4f, 0.0f,

      -0.5f, -0.5f, -0.5f,  0.4f, 0.1f, //bottom
      0.5f, -0.5f, -0.5f,  0.5f, 0.1f,
      0.5f, -0.5f,  0.5f,  0.5f, 0.0f,
      0.5f, -0.5f,  0.5f,  0.5f, 0.0f,
      -0.5f, -0.5f,  0.5f,  0.4f, 0.0f,
      -0.5f, -0.5f, -0.5f,  0.4f, 0.1f,


      -0.5f,  0.5f, -0.5f,  0.5f, 0.1f, //up
      0.5f,  0.5f, -0.5f,  0.6f, 0.1f,
      0.5f,  0.5f,  0.5f,  0.6f, 0.0f,
      0.5f,  0.5f,  0.5f,  0.6f, 0.0f,
      -0.5f,  0.5f,  0.5f,  0.5f, 0.0f,
      -0.5f,  0.5f, -0.5f,  0.5f, 0.1f

   };

   glGenVertexArrays(1, &chunkVAO);
   glGenBuffers(1, &cubeVBO);

   glBindVertexArray(chunkVAO);
   glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

   //apos
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);
   // texture coord attribute
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);




}

void Chunk::sendDataToVBO()
{
   if (this->getCubesData().size() == 0 || this->cubesPosDataVBO == 0)
   {
      return;
   }

   glBindBuffer(GL_ARRAY_BUFFER, this->cubesPosDataVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(CubeGPUStruct) * this->getCubesData().size(), &this->getCubesData()[0], GL_STATIC_DRAW);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Chunk::render()
{
   if (cubesPosDataVBO == 0)
   {
      return;
   }

   glEnableVertexAttribArray(2);
   glBindBuffer(GL_ARRAY_BUFFER, cubesPosDataVBO); // this attribute comes from a different vertex buffer
   glBufferData(GL_ARRAY_BUFFER, sizeof(CubeGPUStruct) * this->getCubesData().size(), &this->getCubesData()[0], GL_STATIC_DRAW);
   glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(3);
   glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));

   glVertexAttribDivisor(2, 1);
   glVertexAttribDivisor(3, 1);


   glBindVertexArray(this->chunkVAO);
   glBindBuffer(GL_ARRAY_BUFFER, this->cubesPosDataVBO);
   glDrawArraysInstanced(GL_TRIANGLES, 0, 36, this->getCubesData().size());
}

std::vector<CubeGPUStruct>& Chunk::getCubesData()
{
   if (!isCubeDataValid)
   {
      this->generateCubesGPUData();
      this->isCubeDataValid = true;
   }


   return this->cubesGPUData;
}

void Chunk::generateCubesGPUData()
{
   std::vector<CubeGPUStruct> cubes;
   for (size_t i = 0; i < cubesCount; i++)
   {
      if (cubesData[i].destroyed || cubesData[i].facesToRender == 0)
      {
         continue;
      }

      CubeGPUStruct cubeData;
      cubeData.blockType = cubesData[i].blockKind;
      cubeData.position = cubesData[i].position;
      cubes.emplace_back(cubeData);
   }
   this->isCubeDataValid = true;
   this->cubesGPUData = cubes;
}
