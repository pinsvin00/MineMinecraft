#pragma once
#include <cstdint>
#include "glm/glm.hpp"
#include "Cube.h"
#include "PerlinNoise.h"
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <optional>

#define CHUNK_SQUARE_LEN 8
#define CHUNK_ARR_SIZ (CHUNK_SQUARE_LEN)*(CHUNK_SQUARE_LEN)*4

//center
//

struct CubeGPUStruct {
	glm::vec3 position;
	float blockType;
};

struct VBOPool {

   std::vector<unsigned int> vbos;
   size_t siz = 0;
   size_t top = 0;
   VBOPool() = default;
   void clear()
   {
      this->top = 0;
   }
   unsigned int get()
   {
       unsigned int vbo = vbos[top];
       top++;

       return vbo;
   }
   void init(size_t total)
   {
      this->siz = total;
      vbos.reserve(total);
      for (size_t i = 0; i < siz; i++)
      {
         unsigned int vbo = 0;
         glGenBuffers(1, &vbo);
         vbos.push_back(vbo);
      }
   }

};


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
		//this->prepareGPU();
		this->cubesData = new Cube[CHUNK_SIZE_X*CHUNK_SIZE_Y*CHUNK_SIZE_Z];
		this->chunkPos = glm::vec3(0.0f);
		this->chunkIdx = glm::vec3(0.0f);
	}

   void addCube(Cube cube, size_t x, size_t y, size_t z);
   std::optional<Cube*> tryGetCube(int x, int y, int z);
   std::vector<Cube*> getNeighboringCubes(Cube& cb);
   std::vector<Cube*> getChunkBatch(glm::vec3 idx, int batchSize);

   void calculateFace(Cube& cb);
   void calculateFaces();

   bool isCubeDataValid = false;

   static inline unsigned int chunkVAO = 0;
   static inline unsigned int cubeVBO = 0;
   static inline VBOPool vboPool;
   //this vbo is assigned from the pool of vbos
   unsigned int cubesPosDataVBO = 0;

   static void prepareGPU();
   void sendDataToVBO();
   void render();

   std::vector<CubeGPUStruct>& getCubesData();
   std::vector<CubeGPUStruct> cubesGPUData;
   std::vector<CubeGPUStruct> generateCubesGPUData();

};

class World
{
public:
   std::map<int, std::map<int, Chunk*>> chunks;
   std::vector<std::pair<int, int>>* loadedChunksIdxs = nullptr;
   std::vector<Chunk*>* loadedChunks = nullptr;

   void generateTree(int x, int y, int z, Chunk * ch);


   Chunk* chunkAt(int x, int y, bool generate = true);
   Chunk* getChunk(int i, int j, bool generate = true);
   Cube* cubeAt(int x, int y, int z);

   //void addCubeAt(int x, int y, int z, Cube& cube);
   void generateChunk(Chunk* chunk);

   const int chunksCount = UINT16_MAX;
   const int chunkOffset = UINT16_MAX / 2;
   const int middleChunk = UINT16_MAX / 2;


   void init();

   World()
   {
      //this->loadedChunksIdxs = new std::vector<std::pair<int, int>>();
      //this->loadedChunks = new std::vector<Chunk*>();

      //this->init();
   }

};

