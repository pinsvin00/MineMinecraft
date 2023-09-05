#include "Game.h"

Game::Game(KeyboardData& kbData) : kbData(kbData)
{
   srand((uint32_t)time(NULL));
   Crosshair::init();
   Cube::init();

   deltaTime = 0.0f;
   blockShader = new Shader("shader.hlsl", "shader_frag.hlsl"); // you can name your shader files however you like
   crosshairShader = new Shader("crosshair_vert.hlsl", "crosshair_frag.hlsl");
   crosshair = std::make_unique<Crosshair>();

   world = std::make_shared<World>();
   this->loadChunksAt(0, 0);

   auto firstChunk = world->getChunk(0, 0);
   glm::vec3 playerPosition = glm::vec3(1.0f);
   for (size_t i = 0; i < firstChunk->cubesCount; i++)
   {
      auto& cube = firstChunk->cubesData[i];
      if (cube.position.y > 20.0f)
      {
         playerPosition = cube.position + glm::vec3(0.0f, 2.0f, 0.0f);
         break;
      }
   }


   this->player = std::make_unique<Player>(world, playerPosition);
}

void Game::loadChunksAt(int x, int y)
{
   Chunk* chunk = world->getChunk(x, y);
   currentChunk = chunk;

   auto loadedChunksIdxs = new std::vector<std::pair<int,int>>();
   auto loadedChunks = new std::vector<Chunk*>();

   for (int xo = -CHUNK_SQUARE_LEN; xo <= CHUNK_SQUARE_LEN; xo++)
   {
      for (int yo = -CHUNK_SQUARE_LEN; yo <= CHUNK_SQUARE_LEN; yo++)
      {
         loadedChunksIdxs->push_back(
            std::make_pair(x + xo, y + yo)
         );
         Chunk* chunk = world->getChunk(x + xo, y + yo);
         if (!chunk->isGenerated)
         {
            chunk->generateCubes();
         }
         loadedChunks->push_back(chunk);
      }
   }

   delete world->loadedChunks;
   delete world->loadedChunksIdxs;

   world->loadedChunks = loadedChunks;
   world->loadedChunksIdxs = loadedChunksIdxs;

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
   this->processKb(nullptr);

   float currentTime = glfwGetTime();
   this->deltaTime = currentTime - lastTime;
   this->lastTime = currentTime;

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
   std::cout << "New chunk loading!" << std::endl;
   glm::vec2 ppos = glm::vec2(this->player->position.x, this->player->position.z);
   Chunk* currentChunk = nullptr;

   auto &loadedChunks = *world->loadedChunks;
   auto &loadedChunksIdxs = *world->loadedChunksIdxs;

   for (size_t i = 0; i < loadedChunksIdxs.size(); i++)
   {
      //check if the player is in the chunk
      auto chunkC = loadedChunksIdxs[i];
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
   std::vector<Chunk*>* newWorldChunks = new std::vector<Chunk*>();
   std::vector<std::pair<int,int>>* newWorldIdx = new std::vector<std::pair<int,int>>();

   newWorldIdx->reserve(CHUNK_ARR_SIZ);
   newWorldChunks->reserve(CHUNK_ARR_SIZ);

   if (currentChunk != nullptr)
   {
      for (int xo = -CHUNK_SQUARE_LEN; xo <= CHUNK_SQUARE_LEN; xo++)
      {
         for (int yo = -CHUNK_SQUARE_LEN; yo <= CHUNK_SQUARE_LEN; yo++)
         {
            newWorldIdx->push_back(
               std::make_pair(currentChunk->chunkIdx.x + xo, currentChunk->chunkIdx.y + yo)
            );
            Chunk* chunk = world->getChunk(currentChunk->chunkIdx.x + xo, currentChunk->chunkIdx.y + yo);
            if (!chunk->isGenerated)
            {
               chunk->generateCubes();
            }
            newWorldChunks->push_back(chunk);
         }
      }

      this->currentChunk = currentChunk;
   }

   delete world->loadedChunks;
   delete world->loadedChunksIdxs;

   world->loadedChunks     = newWorldChunks;
   world->loadedChunksIdxs = newWorldIdx;

}

void Game::processKb(GLFWwindow* window)
{
    auto cameraDir = player->camera->front;
    cameraDir.y = 0;

    const float playerSpeed = 10.0f;


    cameraDir = glm::normalize(cameraDir);

    if (this->kbData.isKeyPressed(KeyboardData::KEY_W))
    {
        glm::vec3 mov = player->camera->front * playerSpeed * deltaTime;
        //mov.y = 0.0f;
        player->position += mov;
        player->camera->position += mov;
        player->collider->position += mov;
    }
    if (this->kbData.isKeyPressed(KeyboardData::KEY_S))
    {
        glm::vec3 mov = player->camera->front * playerSpeed * deltaTime;
        //mov.y = 0.0f;
        player->position -= mov;
        player->camera->position -= mov;
        player->collider->position -= mov;
    }

    if (this->kbData.isKeyPressed(KeyboardData::KEY_A))
    {
        player->position -= glm::cross(cameraDir, player->camera->cameraUp) * playerSpeed * deltaTime;
        player->camera->position -= glm::cross(cameraDir, player->camera->cameraUp) * playerSpeed * deltaTime;;
        player->collider->position -= glm::cross(cameraDir, player->camera->cameraUp) * playerSpeed * deltaTime;;

    }

    if (this->kbData.isKeyPressed(KeyboardData::KEY_D))
    {
        player->position += glm::cross(cameraDir, player->camera->cameraUp) * playerSpeed * deltaTime;;
        player->camera->position += glm::cross(cameraDir, player->camera->cameraUp) * playerSpeed * deltaTime;;
        player->collider->position += glm::cross(cameraDir, player->camera->cameraUp) * playerSpeed * deltaTime;;
    }

    if (this->kbData.isKeyPressed(KeyboardData::KEY_SPACE))
    {
        player->velocity.y = 2.0f;
        player->isGrounded = false;
    }

    this->kbData.reset();

}

void Game::onMouseMove(GLFWwindow* window, double xpos, double ypos)
{
   player->onMouseMove(window, xpos, ypos);
}

void Game::onMouseClick(GLFWwindow* window, int button, int action, int mods)
{
   player->onMouseClick(window, button, action, mods);
}


void Player::draw() {
   collider->draw();
}

void Player::onMouseMove(GLFWwindow* window, double xpos, double ypos)
{
   camera->mouse_callback(window, (float)xpos, (float)ypos);
}

RayCollisionData Player::getCubeAtGunPoint()
{
   Ray ray(this->position, camera->front);

   Cube closestCube;
   Chunk* cubesChunk = nullptr;
   float collisionDistanceCube = -1.0f;

   for (auto& chunk : *this->world->loadedChunks)
   {
       for (size_t i = 0; i < chunk->cubesCount; i++)
       {
         auto& cube = chunk->cubesData[i];
         auto collisionDis = cube.getCollider().intersect(ray);
         if (!cube.dontDraw && !cube.isInitialized && collisionDis != -1.0f)
         {
            if (closestCube.isInitialized)
            {
               float dn = glm::distance(closestCube.position, position);
               float dc = glm::distance(cube.position, position);
               if (dn > dc)
               {
                  closestCube = cube;
                  cubesChunk = chunk;
                  collisionDistanceCube = collisionDis;
               }
            }
            else
            {
               closestCube = cube;
               cubesChunk = chunk;
               collisionDistanceCube = collisionDis;
            }

         }
      }
   }


   RayCollisionData data(closestCube, cubesChunk, collisionDistanceCube);
   return data;
}

Player::Player(std::shared_ptr<World> world, glm::vec3 position) : world(world)
{
   collider = std::make_unique<Cube>();
   camera = std::make_unique<Camera>();
   
   this->setPosition(position);

}

void Player::setPosition(glm::vec3 pos)
{
   this->position = std::move(pos);
   this->camera->position = std::move(pos);
}

void Player::onMouseClick(GLFWwindow* window, int button, int action, int mods)
{
   if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
   {
      auto collisionData = this->getCubeAtGunPoint();


      if (!collisionData.cube.isInitialized || collisionData.chunk == nullptr)
      {
         return;
      }

      float distance = glm::distance(collisionData.cube.position, this->position);
      if (distance >= 8.0f)
      {
         return;
      }

      collisionData.cube.dontDraw = true;

      auto neighbors = collisionData.chunk->getNeighboringCubes(collisionData.cube);
      for (auto neighbor : neighbors)
      {
          collisionData.chunk->calculateFace(*neighbor);
      }
   }
   if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
   {
      auto collisionData = this->getCubeAtGunPoint();
      auto cube = collisionData.cube;

      //glm::vec3 point = this->position;
      glm::vec3 distance = (this->camera->front * collisionData.collisionDistance);
      glm::vec3 pointCoords = this->position + distance;

      ////find closest face of cube, or check collision with each face?
      std::vector<Box3> faceBoxes = {
         Box3(cube.cubeVerts[Cube::VERT_IDXS::LEFT_UP_1], cube.cubeVerts[Cube::VERT_IDXS::RIGHT_UP_2]), //top
         Box3(cube.cubeVerts[Cube::VERT_IDXS::LEFT_DOWN_1], cube.cubeVerts[Cube::VERT_IDXS::RIGHT_DOWN_2]), //bottom
                  
         Box3(cube.cubeVerts[Cube::VERT_IDXS::LEFT_DOWN_1], cube.cubeVerts[Cube::VERT_IDXS::LEFT_UP_2]), //left
         Box3(cube.cubeVerts[Cube::VERT_IDXS::RIGHT_DOWN_1], cube.cubeVerts[Cube::VERT_IDXS::RIGHT_UP_2]), //right
                  
         Box3(cube.cubeVerts[Cube::VERT_IDXS::LEFT_DOWN_1], cube.cubeVerts[Cube::VERT_IDXS::RIGHT_UP_1]), //front
         Box3(cube.cubeVerts[Cube::VERT_IDXS::LEFT_DOWN_2], cube.cubeVerts[Cube::VERT_IDXS::RIGHT_UP_2]), //back
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
         element.bounds[0] += cube.position;
         element.bounds[1] += cube.position;
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

      auto newCube = Cube();

      newCube.isInitialized = true;
      //render all faces of the cube
      newCube.facesToRender = 0b111111;
      newCube.position = cube.position + directions[faceIdx];
      newCube.idx = cube.idx + directions[faceIdx];
      collisionData.chunk->addCube(newCube, newCube.idx.x, newCube.idx.y, newCube.idx.z);
      //calculate faces that are needed for rendering
      collisionData.chunk->calculateFace(newCube);

   }
}

void Player::move(glm::vec3 transofrmation)
{
   this->collider->position += transofrmation;
   this->camera->position += transofrmation;
   this->collider->position += transofrmation;
}

RayCollisionData::RayCollisionData(Cube& cube, Chunk* chunk, float collisionDistance) : cube(cube)
{
   this->chunk = chunk;
   this->collisionDistance = collisionDistance;
}
