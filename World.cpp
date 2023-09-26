#include "World.h"

void World::generateTree(int x, int y, int z, Chunk * ch)
{
    size_t treeHeight = 7;
    int leavesSq = 3;

    bool generateLeaves = false;
    bool generateWood = true;

    size_t leavesHeight = 0;
    std::set<Chunk*> uniqueChunks;
    Cube c;

    for (size_t i = 1; i <= treeHeight; i++)
    {
        //addcube
        if (treeHeight - i <= 3)
        {
            generateLeaves = true;
        }

        if (generateWood)
        {

            c.blockKind = WOOD;
            c.chunkPosition = glm::vec3(x, y + i, z);
            c.position = glm::vec3(x, y + i, z);

            if (x < 0 && z < 0)
            {
                int a = 0;
            }

            Chunk* ch = this->chunkAt(x, z, false);
            ch->addCube(c, abs(x - ch->chunkPos.x), y + i, abs(z - ch->chunkPos.y));
;
        }

        if (generateLeaves)
        {
            for (int xo = -leavesSq; xo <= leavesSq; xo++)
            {
                for (int zo = -leavesSq; zo <= leavesSq; zo++)
                {
                    if (xo == 0 && zo == 0) continue;
                    Chunk* pch = this->chunkAt(x+xo, z+zo, false);

                    if (ch != pch)
                    {
                        int k = 1;
                        pch->isCubeDataValid = false;
                    }
                    pch = this->chunkAt(x + xo, z + zo, false);

                    c.blockKind = LEAVES;
                    c.chunkPosition = glm::vec3(x, y + i, z);
                    c.position = glm::vec3(x + xo, y + i, z + zo);

                    pch->addCube(c, abs(x + xo - pch->chunkPos.x), y + i, abs(z + zo - pch->chunkPos.y));
                }
            }
            leavesSq -= 1;
            generateWood = false;
        }
    }
}

Chunk* World::chunkAt(int x, int y, bool generate)
{
    x = x < 0 ? x - 15  : x;
    y = y < 0 ? y - 15 : y;

    return this->getChunkByIdx(x/16,y/16, generate);
}

std::optional<Cube*> World::getCubeAt(int x, int y, int z)
{
    auto chunk = this->chunkAt(x, z);
    int mx = abs(x - chunk->chunkPos.x);
    int my = y;
    int mz = abs(z  - chunk->chunkPos.y);
    return chunk->getCubeByChunkPos(mx, my, mz);
   
}

void World::addCube(int x, int y, int z, Cube c)
{
}

std::vector<Cube*> World::getBatch(glm::vec3 position, int batchSize)
{
   std::vector<Cube*> batch;

   for (int x = -batchSize; x <= batchSize; x++)
   {
      for (int y = -batchSize; y <= batchSize; y++)
      {
         for (int z = -batchSize; z <= batchSize; z++)
         {

            glm::vec3 pos = position + glm::vec3(x, y, z);
            auto cube = getCubeAt(pos.x, pos.y, pos.z);
            if (cube.has_value() && !cube.value()->destroyed)
            {
               batch.push_back(cube.value());
            }

         }
      }
   }
   return batch;
}

void World::generateChunk(Chunk* chunk)
{
    if (chunk->isGenerated) return;


    const siv::PerlinNoise perlin{ 123 };
    chunk->isGenerated = true;

    for (size_t i = 0; i < 16; i++)
    {
        for (size_t j = 0; j < 16; j++)
        {
            const double frequency = 1.0f;
            const int32_t octaves = 1;
            const double fx = (frequency / 32.0);
            const double fy = (frequency / 32.0);


            auto height = perlin.octave2D_01((i + chunk->chunkPos.x) * fx, (j + chunk->chunkPos.y) * fy, octaves);
            height *= 30.0;
            height = floor(height);
            //height = (double) clamp(height ,0.0, 20.0);

            Cube c;
            c.blockKind = GRASS;
            c.position = glm::vec3(i + chunk->chunkPos.x, height, j + chunk->chunkPos.y);
            c.chunkPosition = glm::vec3(i, height, j);

            if (height >= 13 && rand() % 100 == 0)
            {
                this->generateTree(i + chunk->chunkPos.x, height, j + chunk->chunkPos.y, chunk);
            }
            chunk->addCube(c, i, (int)height, j);
            for (size_t h = 0; h < height; h++)
            {
                Cube c;
                c.position = glm::vec3(i + chunk->chunkPos.x, h, j + chunk->chunkPos.y);
                if (height - 3 <= h)
                {
                    c.blockKind = DIRT;
                }
                else
                {
                    c.blockKind = STONE;
                }

                c.chunkPosition = glm::vec3(i, h, j);
                chunk->addCube(c, i, h, j);
            }

            if (height <= 9)
            {
               for (size_t h = height; h < 10; h++) 
               {
                  Cube c;
                  c.position = glm::vec3(i + chunk->chunkPos.x, h, j + chunk->chunkPos.y);
                  c.chunkPosition = glm::vec3(i, h, j);
                  c.blockKind = WATER;
                  chunk->addCube(c, i, h, j);
               }

            }

        }
    }

   chunk->calculateFaces();

}

Chunk* World::getChunkByIdx(int i, int j, bool generate)
{
   if (chunks[i].count(j) == 0)
   {
      Chunk* c = new Chunk();
      c->chunkIdx = glm::vec2(i,j);

      c->chunkPos = c->chunkIdx;
      c->chunkPos *= 16;

      chunks[i][j] = c;

      if (generate)
      {
          this->generateChunk(c);
      }


      return c;
   }
   else
   {
       if (chunks[i][j]->isGenerated == false && generate)
       {
           this->generateChunk(chunks[i][j]);
       }
       return chunks[i][j];
   }

}

void World::init()
{
}
