
#pragma once
#include "World.h"
#include "Utils.h"
#include "Camera.h"
#include "Cube.h"
#include <algorithm>
#include "Crosshair.h"
#include <mutex>

const float TIME_SPEED_FACTOR = 0.1;

struct RayCollisionData {
   RayCollisionData(Cube* cube, float collisionDistance);
	Cube* cube = nullptr;
	float collisionDistance = 0.0f;
};


class Player {
public:
   std::shared_ptr<Camera> camera = nullptr;
   std::shared_ptr<RectangularCollider> collider = nullptr;
   std::shared_ptr<World>  world = nullptr;
   int blockToPlaceIdx = 1;

   glm::vec3 position = glm::vec3(1.0f);
   glm::vec3 velocity = glm::vec3(0.0f);
   int healthLevel = 8;

   bool isGrounded = false;
   float deltaTime = 0.0f;

   void draw();
   void onMouseMove(GLFWwindow* window, double xpos, double ypos);
   void move(glm::vec3 transofrmation);
   void setPosition(glm::vec3 position);

   Player(std::shared_ptr<World> world, glm::vec3 position);
};

struct WaterExpandInfo {
   //cube and its power
   static constexpr double TIME_DIFF_EXPAND_WATER = 1.0;
   std::vector<std::pair<Cube*, int>> cubesToExpand;
   float lastTimeExpand = 0.0f;
};

class Game
{
public:
   Chunk* currentChunk = nullptr;
   WaterExpandInfo waterExpandInfo;

   std::shared_ptr<World> world;
   std::unique_ptr<Player> player;
   std::unique_ptr<Crosshair> crosshair;

   //all info to transparent cubes
   std::vector<CubeGPUStruct> transparentCubesInfo;
   Cube* outlinedCube;

   const float gravityAcceleration = 5.0f;
   const float playerInitialJumpSpeed = 3.0f;
   const float playerVelocity = 10.0f;

   bool reloadChunkVBO = false;
   float deltaTime;
   float lastTime;
   int blockToPlaceIdx = 1;

   Shader* crosshairShader;
   Shader* blockShader;
   std::vector<Cube*> cubes;
   KeyboardData& kbData;

   glm::vec3 skyColor;
   glm::vec3 lightDirection;
   float lightIntensity;

   void processGame();
   void processDayNightCycle();


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

   void expandWater(Cube & water, int power);
   RayCollisionData getCubeAtGunPoint();

   Game(KeyboardData& kbData);
};

