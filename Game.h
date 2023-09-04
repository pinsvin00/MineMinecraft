
#pragma once
#include "World.h"
#include "Camera.h"
#include "Cube.h"
#include "Crosshair.h"
#include <mutex>
#define CHUNK_SQUARE_LEN 5

class Controllable {
   void onKbInput(GLFWwindow* window) {};
   void onMouseMove(GLFWwindow* window, double xpos, double ypos) {};
   void onMouseClick(GLFWwindow* window, int button, int action, int mods) {};
};

struct KeyboardData {
private:
	std::bitset<16> pressedKeys;


public:
	static constexpr inline uint8_t KEY_W = 0;
	static constexpr inline uint8_t KEY_S = 1;
	static constexpr inline uint8_t KEY_A = 2;
	static constexpr inline uint8_t KEY_D = 3;
	static constexpr inline uint8_t KEY_SPACE = 4;
	bool isLocked = false;

	KeyboardData()
	{	
		pressedKeys = 0;
	}

	void setKeyPressed(uint8_t key)
	{
		pressedKeys[key] = true;
	}

	void reset()
	{
		pressedKeys.reset();
	}

	bool isKeyPressed(uint8_t key)
	{;
		return pressedKeys[key];
	}

};


struct RayCollisionData {
	RayCollisionData(Cube& cube, Chunk* chunk, float collisionDistance) : cube(cube)
	{
		this->chunk = chunk;
		this->collisionDistance = collisionDistance;
	}

	Cube& cube;
	Chunk* chunk = nullptr;
	float collisionDistance = 0.0f;
};


class Player : public Controllable {
public:
   Camera* camera = nullptr;
   Cube* playerModel = nullptr;
   World* world = nullptr;

   glm::vec3 position = glm::vec3(1.0f);

   glm::vec3 velocity = glm::vec3(0);
   bool isGrounded = false;
   float deltaTime = 0.0f;

   void onKbInput(GLFWwindow* window);
   void draw();
   void onMouseMove(GLFWwindow* window, double xpos, double ypos);
   void onMouseClick(GLFWwindow* window, int button, int action, int mods);
   void move(glm::vec3 transofrmation);

   RayCollisionData getCubeAtGunPoint();

   Player();
};

class Game
{
public:
   Chunk* currentChunk = nullptr;

   World* world;
   Player* player;
   Crosshair* crosshair;

   float deltaTime;

   Shader* crosshairShader;
   Shader* blockShader;
   std::vector<Cube*> cubes;
   KeyboardData& kbData;



   void processGame();

   void findNewCurrentChunk();
   bool pointInChunk(glm::vec2 ppos, Chunk& c);
   void loadChunksAt(int x, int y);

   void processKb(GLFWwindow* window);
   void onMouseMove(GLFWwindow* window, double xpos, double ypos);
   void onMouseClick(GLFWwindow* window, int button, int action, int mods);

   Game(KeyboardData& kbData);
};

