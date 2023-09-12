#include "Game.h"

Game::Game(KeyboardData& kbData) : kbData(kbData)
{
   srand((uint32_t)time(NULL));
   Crosshair::init();
   Chunk::prepareGPU();

   deltaTime = 0.0f;
   blockShader = new Shader("shader.hlsl", "shader_frag.hlsl"); // you can name your shader files however you like
   crosshairShader = new Shader("crosshair_vert.hlsl", "crosshair_frag.hlsl");
   crosshair = std::make_unique<Crosshair>();

   world = std::make_shared<World>();
   Chunk::vboPool.init(CHUNK_ARR_SIZ*4);

   this->loadChunksAt(0, 0);


   auto firstChunk = world->getChunkByIdx(0, 0);
   glm::vec3 playerPosition = glm::vec3(16.0f);
   for (size_t i = 0; i < firstChunk->cubesCount; i++)
   {
      auto& cube = firstChunk->cubesData[i];
      if (cube.position.y > 20.0f)
      {
         playerPosition = cube.position + glm::vec3(0.0f, 2.0f, 0.0f);
         break;
      }
   }

   playerPosition += glm::vec3(0, 1, 0);

   this->player = std::make_unique<Player>(world, playerPosition);
}

void Game::resolveCollisions(bool yIter)
{
    RectangularCollider* collider = this->player->collider.get();
    collider->scale = glm::vec3(0.9f, 1.7f, 0.9f);
    collider->origin = glm::vec3(0.0f, -1.0f, 0.0f);
    collider->position = player->position;

    collider->calculateModel();
    auto verts = collider->getVertices();

    glm::vec3 flat;
    flat.x = ceil(player->position.x);
    flat.y = ceil(player->position.y);
    flat.z = ceil(player->position.z);

    auto chunkBatch = this->world->getBatch(flat, 3);
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
   Chunk* chunk = world->getChunkByIdx(x, y);
   currentChunk = chunk;

   auto loadedChunksIdxs = new std::vector<std::pair<int,int>>();
   auto loadedChunks = new std::vector<Chunk*>();
   Chunk::vboPool.clear();
   for (int xo = -CHUNK_SQUARE_LEN; xo <= CHUNK_SQUARE_LEN; xo++)
   {
      for (int yo = -CHUNK_SQUARE_LEN; yo <= CHUNK_SQUARE_LEN; yo++)
      {
         loadedChunksIdxs->push_back(
            std::make_pair(x + xo, y + yo)
         );
         Chunk* chunk = world->getChunkByIdx(x + xo, y + yo);
         if (!chunk->isGenerated)
         {
             this->world->generateChunk(chunk);
         }
         chunk->cubesPosDataVBO = Chunk::vboPool.get();
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
   std::cout << "NEW CHUNK" << std::endl;
   glm::vec2 ppos = glm::vec2(this->player->position.x, this->player->position.z);
   Chunk* currentChunk = nullptr;

   auto &loadedChunks = *world->loadedChunks;
   auto &loadedChunksIdxs = *world->loadedChunksIdxs;

   for (size_t i = 0; i < loadedChunksIdxs.size(); i++)
   {
      //check if the player is in the chunk
      auto chunkC = loadedChunksIdxs[i];
      Chunk* k = world->getChunkByIdx(chunkC.first, chunkC.second);

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
   Chunk::vboPool.clear();
   if (currentChunk != nullptr)
   {
      for (int xo = -CHUNK_SQUARE_LEN; xo <= CHUNK_SQUARE_LEN; xo++)
      {
         for (int yo = -CHUNK_SQUARE_LEN; yo <= CHUNK_SQUARE_LEN; yo++)
         {
            newWorldIdx->push_back(
               std::make_pair(currentChunk->chunkIdx.x + xo, currentChunk->chunkIdx.y + yo)
            );
            Chunk* chunk = world->getChunkByIdx(currentChunk->chunkIdx.x + xo, currentChunk->chunkIdx.y + yo);
            chunk->cubesPosDataVBO = Chunk::vboPool.get();
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
        player->velocity += mov;
    }
    if (this->kbData.isKeyPressed(KeyboardData::KEY_S))
    {
        glm::vec3 mov = cameraDir * playerSpeed;
        player->velocity -= mov;
    }

    if (this->kbData.isKeyPressed(KeyboardData::KEY_A))
    {
       glm::vec3 mov = glm::cross(cameraDir, player->camera->cameraUp) * playerSpeed;
       player->velocity -= mov;

    }

    if (this->kbData.isKeyPressed(KeyboardData::KEY_D))
    {
       glm::vec3 mov = glm::cross(cameraDir, player->camera->cameraUp) * playerSpeed;
       player->velocity += mov;
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

   auto batch = this->world->getBatch(this->position, 8);

   for (auto& element : batch)
   {
      auto cube = element;
      auto collisionDis = cube->getCollider().intersect(ray);
      if (!cube->destroyed && !cube->isInitialized && collisionDis != -1.0f)
      {
         if (closestCube != nullptr)
         {
            float dn = glm::distance(closestCube->position, position);
            float dc = glm::distance(cube->position, position);
            if (dn > dc)
            {
               closestCube = cube;
               collisionDistanceCube = collisionDis;
            }
         }
         else
         {
            closestCube = cube;
            collisionDistanceCube = collisionDis;
         }

      }
   }


   RayCollisionData data(closestCube, collisionDistanceCube);
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


      if (collisionData.cube == nullptr)
      {
         return;
      }

      float distance = glm::distance(collisionData.cube->position, this->position);
      collisionData.cube->destroyed = true;

      auto neighbors = collisionData.cube->cubesChunk->getNeighboringCubes(*collisionData.cube);
      collisionData.cube->cubesChunk->isCubeDataValid = false;
      collisionData.cube->cubesChunk->sendDataToVBO();
   }
   if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
   {
      auto collisionData = this->getCubeAtGunPoint();
      auto& cube = collisionData.cube;

      //glm::vec3 point = this->position;
      glm::vec3 distance = (this->camera->front * collisionData.collisionDistance);
      glm::vec3 pointCoords = this->position + distance;

      ////find closest face of cube, or check collision with each face?
      std::vector<Box3> faceBoxes = {
         Box3(cube->cubeVerts[Cube::VERT_IDXS::LEFT_UP_1], cube->cubeVerts[Cube::VERT_IDXS::RIGHT_UP_2]), //top
         Box3(cube->cubeVerts[Cube::VERT_IDXS::LEFT_DOWN_1], cube->cubeVerts[Cube::VERT_IDXS::RIGHT_DOWN_2]), //bottom
                  
         Box3(cube->cubeVerts[Cube::VERT_IDXS::LEFT_DOWN_1], cube->cubeVerts[Cube::VERT_IDXS::LEFT_UP_2]), //left
         Box3(cube->cubeVerts[Cube::VERT_IDXS::RIGHT_DOWN_1], cube->cubeVerts[Cube::VERT_IDXS::RIGHT_UP_2]), //right
                  
         Box3(cube->cubeVerts[Cube::VERT_IDXS::LEFT_DOWN_1], cube->cubeVerts[Cube::VERT_IDXS::RIGHT_UP_1]), //front
         Box3(cube->cubeVerts[Cube::VERT_IDXS::LEFT_DOWN_2], cube->cubeVerts[Cube::VERT_IDXS::RIGHT_UP_2]), //back
      };


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
         glm::vec3 vec = element.bounds[0] - element.bounds[1];
         auto perp = glm::cross(vec, glm::vec3(0, 0, 1));

         //adding padding to collider, causes x faced cube sides to be in wrong order min/max
         if (faceIdx == 2 || faceIdx == 3)
         {
            perp = glm::vec3(0.0f);
         }

         element.bounds[0] += cube->position - perp * 0.1f;
         element.bounds[1] += cube->position + perp * 0.1f;
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

      Cube newCube;

      //render all faces of the cube
      newCube.facesToRender = 0b111111;
      newCube.position = cube->position + directions[faceIdx] ;
      newCube.chunkPosition = cube->chunkPosition + directions[faceIdx];

      collisionData.cube->cubesChunk->addCube(newCube, newCube.chunkPosition.x, newCube.chunkPosition.y, newCube.chunkPosition.z);
      collisionData.cube->cubesChunk->calculateFace(newCube);
      collisionData.cube->cubesChunk->sendDataToVBO();

   }
}

void Player::move(glm::vec3 transofrmation)
{
   this->collider->position += transofrmation;
   this->camera->position += transofrmation;
   this->collider->position += transofrmation;
}

RayCollisionData::RayCollisionData(Cube* cube, float collisionDistance) : cube(cube)
{
   this->collisionDistance = collisionDistance;
}
