
#pragma once
#include "World.h"
#include "Camera.h"
#include "Cube.h"
#include "Crosshair.h"
#define CHUNK_SQUARE_LEN 5

class Controllable {
   void onKbInput(GLFWwindow* window) {};
   void onMouseMove(GLFWwindow* window, double xpos, double ypos) {};
   void onMouseClick(GLFWwindow* window, int button, int action, int mods) {};
};

class Entity : public Transformable {
public:
   void draw() {};
};


struct RayCollisionData {
   Cube* cube = nullptr;
   Chunk* chunk = nullptr;
   float collisionDistance = 0.0f;

};


class Player : public Entity, Controllable {
public:
   Camera* camera = nullptr;
   Cube* playerModel = nullptr;
   World* world = nullptr;

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



   void processGame();

   void findNewCurrentChunk();
   bool pointInChunk(glm::vec2 ppos, Chunk& c);
   void loadChunksAt(int x, int y);

   void onKbInput(GLFWwindow* window);
   void onMouseMove(GLFWwindow* window, double xpos, double ypos);
   void onMouseClick(GLFWwindow* window, int button, int action, int mods);

   Game();
};

