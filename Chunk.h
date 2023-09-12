#pragma once

#include <cstdint>
#include "glm/glm.hpp"
#include "Cube.h"
#include <vector>
#include "Utils.h"
#include <map>
#include <algorithm>
#include <optional>

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
	Cube* cubesMap[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z] = { nullptr };

	size_t cubesCount = 0;

	Chunk()
	{
		glGenBuffers(1, &this->cubesPosDataVBO);
		//this->prepareGPU();
		this->cubesData = new Cube[CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z];
		this->chunkPos = glm::vec3(0.0f);
		this->chunkIdx = glm::vec3(0.0f);
	}

	void addCube(Cube cube, size_t x, size_t y, size_t z);
	std::optional<Cube*> getCubeByChunkPos(int x, int y, int z);
	std::vector<Cube*> getNeighboringCubes(Cube& cb);

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
	void generateCubesGPUData();

};