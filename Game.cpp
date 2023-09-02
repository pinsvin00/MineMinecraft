#include "Game.h"
#define CHUNK_SQUARE_LEN 10

Game::Game()
{
   srand(time(NULL));
   Crosshair::init();
   Cube::init();

   blockShader = new Shader("shader.hlsl", "shader_frag.hlsl"); // you can name your shader files however you like
   crosshairShader = new Shader("crosshair_vert.hlsl", "crosshair_frag.hlsl");
   world = new World();
   world->init();
   player = new Player();
   crosshair = new Crosshair();
   this->player->world = world;
   deltaTime = 0.0f;
   this->loadChunksAt(0, 0);
}

void Game::loadChunksAt(int x, int y)
{
   Chunk* chunk = world->getChunk(x, y);
   currentChunk = chunk;

   for (int xo = -CHUNK_SQUARE_LEN; xo <= CHUNK_SQUARE_LEN; xo++)
   {
      for (int yo = -CHUNK_SQUARE_LEN; yo <= CHUNK_SQUARE_LEN; yo++)
      {
         world->loadedChunksIdxs.push_back(
            std::make_pair(x + xo, y + yo)
         );
         Chunk* chunk = world->getChunk(x + xo, y + yo);
         if (!chunk->isGenerated)
         {
            chunk->generateCubes();
         }
         world->loadedChunks.push_back(chunk);
      }
   }
}

inline bool Game::pointInChunk(glm::vec2 ppos, Chunk& c)
{
   glm::vec2 ld = c.chunkPos;
   //right up coord of the chunk
   glm::vec2 ru = c.chunkPos + glm::vec2(16, 16);
   bool inChunk = (
      ld.x <= ppos.x &&
      ld.y <= ppos.y &&
      ru.x >= ppos.x &&
      ru.y >= ppos.y
      );
   return inChunk;

}

void Game::processGame()
{
   player->deltaTime = deltaTime;
   if (player->velocity.y != 0.0f)
   {
      player->position.y += player->velocity.y * deltaTime;
      player->camera->position.y += player->velocity.y * deltaTime;
      player->velocity.y -= 1.0f * deltaTime;
   }

   if (player->position.y < 0.0f)
   {
      player->isGrounded = true;
      player->velocity.y = 0.0f;
      player->velocity.y = 0.0f;
   }

   glm::vec2 ppos = glm::vec2(player->position.x, player->position.z);
   if (currentChunk == nullptr || !pointInChunk(ppos, *currentChunk))
   {
      this->findNewCurrentChunk();
   }

}

void Game::findNewCurrentChunk()
{
   glm::vec2 ppos = glm::vec2(this->player->position.x, this->player->position.z);
   Chunk* currentChunk = nullptr;
   for (size_t i = 0; i < world->loadedChunksIdxs.size(); i++)
   {
      //check if the player is in the chunk
      auto chunkC = world->loadedChunksIdxs[i];
      Chunk* k = world->getChunk(chunkC.first, chunkC.second);

      //left down coord of the chunk
      glm::vec2 ld = k->chunkPos;
      //right up coord of the chunk
      glm::vec2 ru = k->chunkPos + glm::vec2(16.0f, 16.0f);


      if (this->pointInChunk(ppos, *k))
      {
         currentChunk = k;
      }
   }
   world->loadedChunks.clear();
   world->loadedChunksIdxs.clear();


   if (currentChunk != nullptr)
   {
      for (int xo = -CHUNK_SQUARE_LEN; xo <= CHUNK_SQUARE_LEN; xo++)
      {
         for (int yo = -CHUNK_SQUARE_LEN; yo <= CHUNK_SQUARE_LEN; yo++)
         {
            world->loadedChunksIdxs.push_back(
               std::make_pair(currentChunk->chunkIdx.x + xo, currentChunk->chunkIdx.y + yo)
            );
            Chunk* chunk = world->getChunk(currentChunk->chunkIdx.x + xo, currentChunk->chunkIdx.y + yo);
            if (!chunk->isGenerated)
            {
               chunk->generateCubes();
            }
            world->loadedChunks.push_back(chunk);
         }
      }

      this->currentChunk = currentChunk;
   }




}

void Game::onKbInput(GLFWwindow* window)
{
   player->onKbInput(window);
}

void Game::onMouseMove(GLFWwindow* window, double xpos, double ypos)
{
   player->onMouseMove(window, xpos, ypos);
}

void Game::onMouseClick(GLFWwindow* window, int button, int action, int mods)
{
   player->onMouseClick(window, button, action, mods);
}

void Player::onKbInput(GLFWwindow* window)
{
   auto cameraDir = camera->front;
   cameraDir.y = 0;
   cameraDir = glm::normalize(cameraDir);

   if (glfwGetKey(window, GLFW_KEY_W))
   {
      glm::vec3 mov = camera->front * 10.0f * deltaTime;
      //mov.y = 0.0f;
      position += mov;
      camera->position += mov;
      playerModel->position += mov;
   }
   if (glfwGetKey(window, GLFW_KEY_S))
   {
      glm::vec3 mov = camera->front * 10.0f * deltaTime;
      //mov.y = 0.0f;
      position -= mov;
      camera->position -= mov;
      playerModel->position -= mov;
   }

   if (glfwGetKey(window, GLFW_KEY_A))
   {
      position -= glm::cross(cameraDir, camera->cameraUp) * 10.0f * deltaTime;
      camera->position -= glm::cross(cameraDir, camera->cameraUp) * 10.0f * deltaTime;;
      playerModel->position -= glm::cross(cameraDir, camera->cameraUp) * 10.0f * deltaTime;;

   }

   if (glfwGetKey(window, GLFW_KEY_D))
   {
      position += glm::cross(cameraDir, camera->cameraUp) * 10.0f * deltaTime;;
      camera->position += glm::cross(cameraDir, camera->cameraUp) * 10.0f * deltaTime;;
      playerModel->position += glm::cross(cameraDir, camera->cameraUp) * 10.0f * deltaTime;;
   }

   if (glfwGetKey(window, GLFW_KEY_SPACE))
   {
      velocity.y = 2.0f;
      isGrounded = false;
   }

}

void Player::onMouseMove(GLFWwindow* window, double xpos, double ypos)
{
   camera->mouse_callback(window, xpos, ypos);
}

RayCollisionData Player::getCubeAtGunPoint()
{
   Ray ray(this->position, camera->front);

   Cube* closestCube = nullptr;
   Chunk* cubesChunk = nullptr;
   float collisionDistanceCube = -1.0f;

   for (auto& chunk : this->world->loadedChunks)
   {
      for (auto& element : chunk->cubes)
      {
         auto collisionDis = element->getCollider().intersect(ray);
         if (!element->dontDraw && collisionDis != -1.0f)
         {
            if (closestCube != nullptr)
            {
               float dn = glm::distance(closestCube->position, position);
               float dc = glm::distance(element->position, position);
               if (dn > dc)
               {
                  closestCube = element;
                  cubesChunk = chunk;
                  collisionDistanceCube = collisionDis;
               }
            }
            else
            {
               closestCube = element;
               cubesChunk = chunk;
               collisionDistanceCube = collisionDis;
            }

         }
      }
   }


   RayCollisionData data;
   data.chunk = cubesChunk;
   data.cube = closestCube;
   data.collisionDistance = collisionDistanceCube;

   return data;
}

void Player::onMouseClick(GLFWwindow* window, int button, int action, int mods)
{
   if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
   {
      auto collisionData = this->getCubeAtGunPoint();


      if (collisionData.cube == nullptr || collisionData.chunk == nullptr)
      {
         return;
      }

      float distance = glm::distance(collisionData.cube->position, this->position);
      if (distance >= 8.0f)
      {
         return;
      }

      collisionData.cube->dontDraw = true;

      auto neighbors = collisionData.chunk->getNeighboringCubes(collisionData.cube);
      for (auto& neighbor : neighbors)
      {
         if (neighbor != nullptr)
         {
            collisionData.chunk->calculateFace(neighbor);
         }
      }
   }
   if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
   {
      auto collisionData = this->getCubeAtGunPoint();
      auto cube = collisionData.cube;

      //glm::vec3 point = this->position;
      glm::vec3 distance = (this->camera->front * collisionData.collisionDistance);
      glm::vec3 pointCoords = this->position + distance;

      cube1->position = pointCoords;
      cube1->scale = glm::vec3(0.1f, 0.1f, 0.1f);

      ////find closest face of cube, or check collision with each face?
      std::vector<Box3> faceBoxes = {
         Box3(cube->verts[Cube::VERT_IDXS::LEFT_UP_1], cube->verts[Cube::VERT_IDXS::RIGHT_UP_2]), //top
         Box3(cube->verts[Cube::VERT_IDXS::LEFT_DOWN_1], cube->verts[Cube::VERT_IDXS::RIGHT_DOWN_2]), //bottom

         Box3(cube->verts[Cube::VERT_IDXS::LEFT_DOWN_1], cube->verts[Cube::VERT_IDXS::LEFT_UP_2]), //left
         Box3(cube->verts[Cube::VERT_IDXS::RIGHT_DOWN_1], cube->verts[Cube::VERT_IDXS::RIGHT_UP_2]), //right

         Box3(cube->verts[Cube::VERT_IDXS::LEFT_DOWN_1], cube->verts[Cube::VERT_IDXS::RIGHT_UP_1]), //front
         Box3(cube->verts[Cube::VERT_IDXS::LEFT_DOWN_2], cube->verts[Cube::VERT_IDXS::RIGHT_UP_2]), //back
      };

      const int TOP = 0;
      const int BOTTOM = 1;
      const int LEFT = 2;
      const int RIGHT = 3;
      const int FRONT = 4;
      const int BACK = 5;


      std::vector<glm::vec3> directions = {
         glm::vec3(0 ,1 ,0),
         glm::vec3(0 ,-1,0),
         glm::vec3(-1,0 ,0),
         glm::vec3(1 ,0 ,0),
         glm::vec3(0 ,0 ,-1),
         glm::vec3(0 ,0, 1),
      };

      size_t faceIdx = 0;
      for (auto element : faceBoxes)
      {
         element.bounds[0] += cube->position;
         element.bounds[1] += cube->position;
         if (element.intersect(pointCoords))
         {
            break;
         }
         faceIdx++;
      }

      if (faceIdx == 6)
      {
         return;
      }

      auto newCube = new Cube();

      //render all faces of the cube
      newCube->facesToRender = 0b111111;
      newCube->position = cube->position + directions[faceIdx];
      newCube->idx = cube->idx + directions[faceIdx];
      collisionData.chunk->cubes.push_back(newCube);
      //calculate faces that are needed for rendering
      collisionData.chunk->calculateFace(newCube);

   }
}

void Player::move(glm::vec3 transofrmation)
{
   this->playerModel->position += transofrmation;
   this->camera->position += transofrmation;
   this->playerModel->position += transofrmation;
}
