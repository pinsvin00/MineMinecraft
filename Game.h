
#pragma once
#include "World.h"
#include "Camera.h"
#include "Cube.h"
#include "Crosshair.h"
#include <mutex>S

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
   RayCollisionData(Cube& cube, Chunk* chunk, float collisionDistance);
	Cube& cube;
	Chunk* chunk = nullptr;
	float collisionDistance = 0.0f;
};


class Player {
public:
   std::unique_ptr<Camera> camera = nullptr;
   std::unique_ptr<Cube>   collider = nullptr;
   std::shared_ptr<World>  world = nullptr;

   glm::vec3 position = glm::vec3(1.0f);

   glm::vec3 velocity = glm::vec3(0.0f);
   bool isGrounded = false;
   float deltaTime = 0.0f;

   void draw();
   void onMouseMove(GLFWwindow* window, double xpos, double ypos);
   void onMouseClick(GLFWwindow* window, int button, int action, int mods);
   void move(glm::vec3 transofrmation);
   void setPosition(glm::vec3 position);

   RayCollisionData getCubeAtGunPoint();
   Player(std::shared_ptr<World> world, glm::vec3 position);
};

class Game
{
public:
   Chunk* currentChunk = nullptr;

   std::shared_ptr<World> world;
   std::unique_ptr<Player> player;
   std::unique_ptr<Crosshair> crosshair;

   float deltaTime;
   float lastTime;

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

