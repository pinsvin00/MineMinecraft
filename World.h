#pragma once
#include <cstdint>
#include "glm/glm.hpp"
#include "Chunk.h"
#include "Utils.h"
#include "Cube.h"
#include "PerlinNoise.h"
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <optional>

#define CHUNK_SQUARE_LEN 8
#define CHUNK_ARR_SIZ (CHUNK_SQUARE_LEN)*(CHUNK_SQUARE_LEN)*4

class World
{
public:
   std::map<int, std::map<int, Chunk*>> chunks;
   std::vector<std::pair<int, int>>* loadedChunksIdxs = nullptr;
   std::vector<Chunk*>* loadedChunks = nullptr;
   const int chunksCount = UINT16_MAX;
   const int chunkOffset = UINT16_MAX / 2;
   const int middleChunk = UINT16_MAX / 2;


   void generateTree(int x, int y, int z, Chunk * ch);


   Chunk* chunkAt(int x, int y, bool generate = true);
   Chunk* getChunkByIdx(int i, int j, bool generate = true);
   std::optional<Cube*> getCubeAt(int x, int y, int z);
   void addCube(int x, int y, int z, Cube c);

   std::vector<Cube*> getBatch(glm::vec3 position, int batchSize);
   void generateChunk(Chunk* chunk);

   void init();

   World() = default;

};

