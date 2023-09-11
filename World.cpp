#include "World.h"

void Chunk::addCube(Cube cube, size_t x, size_t y, size_t z)
{
    this->isCubeDataValid = false;
    this->cubesData[cubesCount] = std::move(cube);
    this->cubesMap[x][y][z] = &cubesData[cubesCount];
    this->cubesCount += 1;
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
      glm::vec3 neighborPositon = cb.chunkPosition + direction;
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
      auto cube = tryGetCube(
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
            if (cube.has_value() && !cube.value()->destroyed)
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
   //
   //glEnableVertexAttribArray(2);
   ////glBindBuffer(GL_ARRAY_BUFFER, cubesPosDataVBO); // this attribute comes from a different vertex buffer
   //glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
   //glEnableVertexAttribArray(3);
   //glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));

   //glVertexAttribDivisor(2, 1);
   //glVertexAttribDivisor(3, 1);



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

;

   glBindVertexArray(this->chunkVAO);
   glBindBuffer(GL_ARRAY_BUFFER, this->cubesPosDataVBO);
   glDrawArraysInstanced(GL_TRIANGLES, 0, 36, this->getCubesData().size());
}

std::vector<CubeGPUStruct>& Chunk::getCubesData()
{
   if (!isCubeDataValid)
   {
      this->cubesGPUData = this->generateCubesGPUData();
      this->isCubeDataValid = true;
   }
   

   return this->cubesGPUData;
}

std::vector<CubeGPUStruct> Chunk::generateCubesGPUData()
{
   std::vector<CubeGPUStruct> cubes;
   for (size_t i = 0; i < cubesCount; i++)
   {
      if (cubesData[i].destroyed)
      {
          continue;
      }

      CubeGPUStruct cubeData;
      cubeData.blockType = cubesData[i].blockKind;
      cubeData.position = cubesData[i].position;
      cubes.emplace_back(cubeData);
   }

   return cubes;
}

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

    return this->getChunk(x/16,y/16, generate);
}

Cube* World::cubeAt(int x, int y, int z)
{
    auto chunk = this->chunkAt(x, y);

    int mx = x % 16;
    int my = y % 16;
    int mz = z % 16;

    return chunk->cubesMap[mx][my][mz];
   
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
            c.processMat();

            if (height >= 10 && rand() % 100 == 0)
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
                c.processMat();
                chunk->addCube(c, i, h, j);
            }

        }
    }
}

Chunk* World::getChunk(int i, int j, bool generate)
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
