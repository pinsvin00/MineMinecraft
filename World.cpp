#include "World.h"

void Chunk::addCube(Cube cube, size_t x, size_t y, size_t z)
{
    this->isCubeDataValid = false;
    this->cubesData[cubesCount] = std::move(cube);
    this->cubesMap[x][y][z] = &cubesData[cubesCount];
    this->cubesCount += 1;
}

void Chunk::generateCubes()
{
    if (isGenerated) return;


    const siv::PerlinNoise perlin{ 123 };

    for (size_t i = 0; i < 16; i++)
    {
        for (size_t j = 0; j < 16; j++)
        {
            const double frequency = 1.0f;
            const int32_t octaves = 1;
            const double fx = (frequency / 16.0);
            const double fy = (frequency / 16.0);


            auto height = perlin.octave2D_01((i + chunkPos.x) * fx, (j + chunkPos.y) * fy, octaves);
            height *= 30.0;
            height = floor(height);
            height = (double) clamp(height ,0.0, 20.0);

            Cube c;
            c.position = glm::vec3(i + chunkPos.x, height, j + chunkPos.y);
            c.idx = glm::vec3(i, height, j);
            c.processMat();

            this->addCube(c, i, (int)height, j);
            for (size_t h = 0; h < height; h++)
            {
                Cube c;
                c.position = glm::vec3(i + chunkPos.x, h, j + chunkPos.y);
                c.idx = glm::vec3(i, h, j);
                c.processMat();
                addCube(c, i, h, j);
            }

        }
    }
    isGenerated = true;

    calculateFaces();
    this->sendDataToVBO();
}

std::optional<Cube*> Chunk::tryGetCube(int x, int y, int z)
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
      glm::vec3 neighborPositon = cb.idx + direction;
      auto neighborCube = tryGetCube(neighborPositon.x, neighborPositon.y, neighborPositon.z);
      if (neighborCube.has_value())
      {
          result.emplace_back(neighborCube.value());
      }
   }


   return result;
}

void Chunk::calculateFace(Cube& cb)
{
   auto idx = cb.idx;
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
      auto cube = tryGetCube(
          (int)pos.x,
          (int)pos.y,
          (int)pos.z
      );

      if (cube.has_value() == false || cube.value()->dontDraw == true)
      {
         cb.facesToRender |= 1 << iter;
      }
      iter++;
   }
}

void Chunk::calculateFaces()
{
    for (size_t i = 0; i < cubesCount; i++)
    {
        calculateFace(cubesData[i]);
    }
}


std::vector<Cube*> Chunk::getChunkBatch(glm::vec3 position, int batchSize)
{
   std::vector<Cube*> batch;
   position -= glm::vec3(chunkPos.x, 0, chunkPos.y);
   //batch.reserve(batchSize * batchSize * batchSize * 8);

   for (int x = -batchSize; x <= batchSize ; x++)
   {
      for (int y = -batchSize; y <= batchSize ; y++)
      {
         for (int z = -batchSize; z <= batchSize ; z++)
         {
            auto cube = this->tryGetCube(position.x + x, position.y + y, position.z + z);
            if (cube.has_value())
            {
               batch.push_back(cube.value());
            }

         }
      }
   }
   return batch;
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

   glGenVertexArrays(1, &this->chunkVAO);
   glGenBuffers(1, &this->chunkVBO);
   glGenBuffers(1, &this->cubesPosDataVBO);

   glBindVertexArray(this->chunkVAO);
   glBindBuffer(GL_ARRAY_BUFFER, this->chunkVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

   //apos
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);
   // texture coord attribute
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);
   //
   glEnableVertexAttribArray(2);
   glBindBuffer(GL_ARRAY_BUFFER, this->cubesPosDataVBO); // this attribute comes from a different vertex buffer
   glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.



}

void Chunk::sendDataToVBO()
{
   if (this->getCubesData().size() == 0)
   {
      return;
   }

   glBindBuffer(GL_ARRAY_BUFFER, this->cubesPosDataVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->getCubesData().size(), &this->getCubesData()[0], GL_STATIC_DRAW);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Chunk::render()
{
   glBindVertexArray(this->chunkVAO);
   glDrawArraysInstanced(GL_TRIANGLES, 0, 36, cubesCount);
}

std::vector<glm::vec3>& Chunk::getCubesData()
{
   if (!isCubeDataValid)
   {
      this->cubesGPUData = this->generateCubesGPUData();
      this->isCubeDataValid = true;
   }
   

   return this->cubesGPUData;
}

std::vector<glm::vec3> Chunk::generateCubesGPUData()
{
   std::vector<glm::vec3> cubes;
   for (size_t i = 0; i < cubesCount; i++)
   {
      cubes.emplace_back(cubesData[i].position);
   }

   return cubes;
}

Chunk* World::getChunk(int i, int j)
{
   glm::vec2 idx(i, j);
   if (chunks[idx.x].count(idx.y) == 0)
   {
      Chunk* c = new Chunk();
      c->chunkIdx = idx;

      c->chunkPos = idx;
      c->chunkPos *= 16;

      c->generateCubes();
      chunks[(int)idx.x][(int)idx.y] = c;
      return c;
   }
   else
   {
      return chunks[idx.x][idx.y];
   }

}

void World::init()
{
}
