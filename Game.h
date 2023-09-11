
#pragma once
#include "World.h"
#include "Camera.h"
#include "Cube.h"
#include <algorithm>
#include "Crosshair.h"
#include <mutex>

class Controllable {
   void onKbInput(GLFWwindow* window) {};
   void onMouseMove(GLFWwindow* window, double xpos, double ypos) {};
   void onMouseClick(GLFWwindow* window, int button, int action, int mods) {};
};

struct KeyboardData {
private:
public:
	std::bitset<16> pressedKeys;
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

	void setKeyPressed(uint8_t key, bool isPressed)
	{
		pressedKeys[key] = isPressed;
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
   std::shared_ptr<RectangularCollider> collider = nullptr;
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

   const float gravityAcceleration = 3.0f;

   const float playerInitialJumpSpeed = 5.0f;
   const float playerVelocity = 10.0f;


   float deltaTime;
   float lastTime;

   Shader* crosshairShader;
   Shader* blockShader;
   std::vector<Cube*> cubes;
   KeyboardData& kbData;

   Cube specialCube;

   void processGame();

   void findNewCurrentChunk();
   bool pointInChunk(glm::vec2 ppos, Chunk& c);
   void loadChunksAt(int x, int y);

   void processKb(GLFWwindow* window);
   void onMouseMove(GLFWwindow* window, double xpos, double ypos);
   void onMouseClick(GLFWwindow* window, int button, int action, int mods);
   /// <summary>
   /// This function resolve collisions in given axes, after running it there shouldn't be any collisions
   /// </summary>
   void resolveCollisions(bool yIter);

   Game(KeyboardData& kbData);
};

