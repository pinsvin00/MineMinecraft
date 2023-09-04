#include "World.h"

void Chunk::generateCubes()
{
    if (isGenerated) return;


    const siv::PerlinNoise perlin{ 123 };

    for (size_t i = 0; i < 16; i++)
    {
        for (size_t j = 0; j < 16; j++)
        {
            //for (size_t z = 0; z <= 0; z++)
            //{

            const double frequency = 1.0f;
            const int32_t octaves = 40;
            const double fx = (frequency / 16.0);
            const double fy = (frequency / 16.0);


            auto height = perlin.octave2D_01((i + chunkPos.x) * fx, (j + chunkPos.y) * fy, octaves);
            height *= 40.0;
            height = floor(height);


            Cube* c = new Cube();
            c->position = glm::vec3((float)i + chunkPos.x, (float)height, (float)j + chunkPos.y);
            c->idx = glm::vec3(i, height, j);
            c->processMat();

            cubeMap[i][(int)height][j] = c;
            cubes.push_back(c);

            for (size_t h = 0; h < height; h++)
            {
                Cube* c = new Cube();
                c->position = glm::vec3((float)i + chunkPos.x, (float)h, (float)j + chunkPos.y);
                c->idx = glm::vec3(i, h, j);
                c->processMat();
                cubeMap[i][h][j] = c;
                cubes.push_back(c);
            }

            //}

        }
    }
    isGenerated = true;

    calculateFaces();
}

Cube* Chunk::tryGetCube(int x, int y, int z)
{
   if (x < 0 || x >= 16)
   {
      return nullptr;
   }
   if (z < 0 || z >= 16)
   {
      return nullptr;
   }
   if (y < 0 || y >= 64)
   {
      return nullptr;
   }

   return cubeMap[x][y][z];
}

std::vector<Cube*> Chunk::getNeighboringCubes(Cube* cb)
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
      glm::vec3 neighborPositon = cb->idx + direction;
      auto neighborCube = tryGetCube(neighborPositon.x, neighborPositon.y, neighborPositon.z);
      result.push_back(neighborCube);
   }


   return result;
}

void Chunk::calculateFace(Cube* cb)
{
   auto idx = cb->idx;
   std::vector<glm::vec3> dirs = {
      glm::vec3(0,0,-1),
      glm::vec3(0,0 ,1),
      glm::vec3(-1,0,0),
      glm::vec3(1,0,0),
      glm::vec3(0,1,0),
      glm::vec3(0,1,0),
   };


   size_t iter = 0;
   cb->facesToRender = 0;
   for (auto& element : dirs)
   {
      glm::vec3 pos = idx + element;
      auto cube = tryGetCube(pos.x, pos.y, pos.z);

      if (cube == nullptr || cube->dontDraw == true)
      {
         cb->facesToRender |= 1 << iter;
      }
      iter++;
   }
}

void Chunk::calculateFaces()
{
   for (auto& cb : cubes)
   {
      calculateFace(cb);
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
