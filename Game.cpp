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
   this->loadChunksAt(2, 2);

   auto firstChunk = world->getChunk(2, 2);
   glm::vec3 playerPosition = glm::vec3(16.0f);
   //for (size_t i = 0; i < firstChunk->cubesCount; i++)
   //{
   //   auto& cube = firstChunk->cubesData[i];
   //   if (cube.position.y > 20.0f)
   //   {
   //      playerPosition = cube.position + glm::vec3(0.0f, 2.0f, 0.0f);
   //      break;
   //   }
   //}

   currentChunk->sendDataToVBO();
   playerPosition += glm::vec3(0, 1, 0);

   this->player = std::make_unique<Player>(world, playerPosition);
}

void Game::resolveCollisions(bool yIter)
{
    RectangularCollider* collider = this->player->collider.get();
    collider->scale = glm::vec3(1.0f, 2.0f, 1.0f);
    collider->origin = glm::vec3(0.0f, -1.0f, 0.0f);
    collider->position = player->position;

    collider->calculateModel();
    auto verts = collider->getVertices();

    glm::vec3 flat;
    flat.x = ceil(player->position.x);
    flat.y = ceil(player->position.y);
    flat.z = ceil(player->position.z);

    auto chunkBatch = currentChunk->getChunkBatch(flat, 2);
    for (auto& element : chunkBatch)
    {
         auto &cube = *element;
         if (cube.checkCollision(collider))
         {
             player->position -= player->velocity * deltaTime;
         
             if (yIter)
             {
                 player->position.y += 0.001f;
                 player->isGrounded = true;
                 player->velocity.y = 0.0f;
             }
         
             break;
         }
    }
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
             this->world->generateChunk(chunk);
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

   float currentTime = glfwGetTime();
   this->deltaTime = currentTime - lastTime;
   this->lastTime = currentTime;

   this->deltaTime = clamp((double)deltaTime, 0.0, 0.5);

   player->deltaTime = deltaTime;

   //try to move by kb
   this->player->velocity.x = 0.0f;
   float preservedY = player->velocity.y;
   this->player->velocity.y = 0.0f;


   this->processKb(nullptr);

   //resolve collisions in xz axes
   player->position += player->velocity * deltaTime;
   this->resolveCollisions(false);

   //resolve collisions in y axis
   player->velocity = glm::vec3(0.0f);
   player->velocity.y = preservedY;
   player->velocity.y -= gravityAcceleration * deltaTime;
   player->position += player->velocity * deltaTime;

   this->resolveCollisions(true);

   if (this->kbData.isKeyPressed(KeyboardData::KEY_SPACE))
   {
       player->velocity.y = playerInitialJumpSpeed;
       player->isGrounded = false;
   }


   glm::vec2 ppos = glm::vec2(player->position.x, player->position.z);
   if (currentChunk == nullptr || !pointInChunk(ppos, *currentChunk))
   {
      this->findNewCurrentChunk();
   }

   //update player dependent positions
   player->collider->position = player->position;
   player->camera->position = player->position;

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
               world->generateChunk(chunk);
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
    const float playerSpeed = 5.0f;

    cameraDir.y = 0.0f;
    cameraDir = glm::normalize(cameraDir);

    if (this->kbData.isKeyPressed(KeyboardData::KEY_W))
    {
        glm::vec3 mov = cameraDir * playerSpeed;
        //std::cout << "Dt = " << deltaTime << std::endl;
        player->velocity = mov;
    }
    if (this->kbData.isKeyPressed(KeyboardData::KEY_S))
    {
        glm::vec3 mov = cameraDir * playerSpeed;
        player->velocity = -mov;
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

}

void Player::onMouseMove(GLFWwindow* window, double xpos, double ypos)
{
   camera->mouse_callback(window, (float)xpos, (float)ypos);
}

RayCollisionData Player::getCubeAtGunPoint()
{
   Ray ray(this->position, camera->front);

   Cube* closestCube = nullptr;
   Chunk* cubesChunk = nullptr;
   float collisionDistanceCube = -1.0f;

   for (auto& chunk : *this->world->loadedChunks)
   {
       for (size_t i = 0; i < chunk->cubesCount; i++)
       {
         auto& cube = chunk->cubesData[i];
         auto collisionDis = cube.getCollider().intersect(ray);
         if (!cube.destroyed && !cube.isInitialized && collisionDis != -1.0f)
         {
            if (closestCube != nullptr)
            {
               float dn = glm::distance(closestCube->position, position);
               float dc = glm::distance(cube.position, position);
               if (dn > dc)
               {
                  closestCube = &cube;
                  cubesChunk = chunk;
                  collisionDistanceCube = collisionDis;
               }
            }
            else
            {
               closestCube = &cube;
               cubesChunk = chunk;
               collisionDistanceCube = collisionDis;
            }

         }
      }
   }


   RayCollisionData data(*closestCube, cubesChunk, collisionDistanceCube);
   return data;
}

Player::Player(std::shared_ptr<World> world, glm::vec3 position) : world(world)
{
   collider = std::make_shared<RectangularCollider>();
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


      if (collisionData.chunk == nullptr)
      {
         return;
      }

      float distance = glm::distance(collisionData.cube.position, this->position);
      if (distance >= 16.0f)
      {
         return;
      }

      collisionData.cube.destroyed = true;

      auto neighbors = collisionData.chunk->getNeighboringCubes(collisionData.cube);
      //for (auto neighbor : neighbors)
      //{
      //    collisionData.chunk->calculateFace(*neighbor);
      //}
      collisionData.chunk->isCubeDataValid = false;
      collisionData.chunk->sendDataToVBO();
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
      newCube.chunkPosition = cube.chunkPosition + directions[faceIdx];
      collisionData.chunk->addCube(newCube, newCube.chunkPosition.x, newCube.chunkPosition.y, newCube.chunkPosition.z);
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
