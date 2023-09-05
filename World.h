#pragma once
#include <cstdint>
#include "glm/glm.hpp"
#include "Cube.h"
#include "PerlinNoise.h"
#include <vector>
#include <map>
#include <optional>

#define CHUNK_SQUARE_LEN 8
#define CHUNK_ARR_SIZ (CHUNK_SQUARE_LEN)*(CHUNK_SQUARE_LEN)*4

//center
//

class Chunk {
public:
	static constexpr inline uint8_t CHUNK_SIZE_X = 16;
	static constexpr inline uint8_t CHUNK_SIZE_Y = 64;
	static constexpr inline uint8_t CHUNK_SIZE_Z = 16;
	static constexpr inline uint16_t CHUNK_ARR_SIZE = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z;

   bool isDummy = false;
   bool isGenerated = false;
   glm::vec2 chunkPos;
   glm::vec2 chunkIdx;

   //memory arena for the cubes, serves also as new cube map
   Cube* cubesData = nullptr;
   //Map for calculations, allowed for some cold reads
   Cube* cubesMap[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z] = {nullptr};

   size_t cubesCount = 0;

   Chunk()
   {
      glGenBuffers(1, &this->cubesPosDataVBO);
      this->prepareGPU();


	   this->cubesData = new Cube[CHUNK_SIZE_X*CHUNK_SIZE_Y*CHUNK_SIZE_Z];
	   this->chunkPos = glm::vec3(0.0f);
	   this->chunkIdx = glm::vec3(0.0f);
   }

   void addCube(Cube cube, size_t x, size_t y, size_t z);
   void generateCubes();
   std::optional<Cube*> tryGetCube(int x, int y, int z);
   std::vector<Cube*> getNeighboringCubes(Cube& cb);
   void calculateFace(Cube& cb);
   void calculateFaces();

   bool isCubeDataValid = false;

   unsigned int chunkVAO = 0;
   unsigned int chunkVBO = 0;
   unsigned int cubesPosDataVBO = 0;

   void prepareGPU();
   void sendDataToVBO();
   void render();

   std::vector<glm::vec3>& getCubesData();
   std::vector<glm::vec3> cubesGPUData;
   std::vector<glm::vec3> generateCubesGPUData();

};

class World
{
public:
   std::map<int, std::map<int, Chunk*>> chunks;
   std::vector<std::pair<int, int>>* loadedChunksIdxs = nullptr;
   std::vector<Chunk*>* loadedChunks = nullptr;

   const int chunksCount = UINT16_MAX;
   const int chunkOffset = UINT16_MAX / 2;
   const int middleChunk = UINT16_MAX / 2;

   Chunk* getChunk(int i, int j);
   void init();

   World()
   {
      //this->loadedChunksIdxs = new std::vector<std::pair<int, int>>();
      //this->loadedChunks = new std::vector<Chunk*>();

      //this->init();
   }

};

