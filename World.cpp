#include "World.h"

void Chunk::addCube(Cube cube, size_t x, size_t y, size_t z)
{

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
            const int32_t octaves = 40;
            const double fx = (frequency / 16.0);
            const double fy = (frequency / 16.0);


            auto height = perlin.octave2D_01((i + chunkPos.x) * fx, (j + chunkPos.y) * fy, octaves);
            height *= 1.0;
            height = floor(height);


            //those cubes will be temporaily on the stack 
            Cube c;
            c.position = glm::vec3((float)i + chunkPos.x, (float)height, (float)j + chunkPos.y);
            c.idx = glm::vec3(i, height, j);
            c.processMat();

            this->addCube(c, i, (int)height, j);
            for (size_t h = 0; h < height; h++)
            {
                Cube c;
                c.position = glm::vec3((float)i + chunkPos.x, (float)h, (float)j + chunkPos.y);
                c.idx = glm::vec3(i, h, j);
                c.processMat();
                addCube(c, i, (int) height, j);
            }

        }
    }
    isGenerated = true;

    calculateFaces();
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
      auto cube = tryGetCube(pos.x, pos.y, pos.z);

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
      chunks[idx.x][idx.y] = c;
      return c;
   }
   else
   {
      return chunks[idx.x][idx.y];
   }

}

void World::init()
{

   for (int i = 0; i < 8; i++)
   {
      for (int j = 0; j < 8; j++)
      {
         auto chunk = new Chunk();
         int realX = i - 4;
         int realY = j - 4;

         chunk->chunkPos = glm::vec2(realX * 16, realY * 16);
         chunk->chunkIdx = glm::vec2(realX, realY);
         chunk->generateCubes();
         chunks[i - 4][j - 4] = chunk;
      }
   }
}
