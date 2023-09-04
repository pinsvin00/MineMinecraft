#pragma once
#include <cstdint>
#include "glm/glm.hpp"
#include "Cube.h"
#include "PerlinNoise.h"
#include <vector>
#include <map>



class Chunk {
public:
   bool isDummy = false;
   bool isGenerated = false;
   uint8_t blocks[16][16][16];
   glm::vec2 chunkPos;
   glm::vec2 chunkIdx;
   Cube* cubeMap[16][64][16] = { nullptr };
   std::vector<Cube*> cubes;

   Chunk()
   {
   }

   void generateCubes();
   Cube* tryGetCube(int x, int y, int z);
   std::vector<Cube*> getNeighboringCubes(Cube* cb);
   void calculateFace(Cube* cb);
   void calculateFaces();
};

class World
{
public:
   std::map<int, std::map<int, Chunk*>> chunks;
   std::vector<std::pair<int, int>> loadedChunksIdxs;
   std::vector<Chunk*> loadedChunks;

   int chunksCount = UINT16_MAX;
   int chunkOffset = UINT16_MAX / 2;
   int middleChunk = UINT16_MAX / 2;

   Chunk* getChunk(int i, int j);
   void init();

};

