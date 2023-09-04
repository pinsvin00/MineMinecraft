#include "Game.h"

Game::Game(KeyboardData& kbData) : kbData(kbData)
{
   srand((uint32_t)time(NULL));
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
   this->processKb(nullptr);
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

void Game::processKb(GLFWwindow* window)
{
    auto cameraDir = player->camera->front;
    cameraDir.y = 0;

    const float playerSpeed = 0.001f;

    this->kbData.isLocked = true;

    //cameraDir = glm::normalize(cameraDir);w

    if (this->kbData.isKeyPressed(KeyboardData::KEY_W))
    {
        glm::vec3 mov = player->camera->front * playerSpeed * deltaTime;
        //mov.y = 0.0f;
        player->position += mov;
        player->camera->position += mov;
        player->playerModel->position += mov;
    }
    if (this->kbData.isKeyPressed(KeyboardData::KEY_S))
    {
        glm::vec3 mov = player->camera->front * playerSpeed * deltaTime;
        //mov.y = 0.0f;
        player->position -= mov;
        player->camera->position -= mov;
        player->playerModel->position -= mov;
    }

    if (this->kbData.isKeyPressed(KeyboardData::KEY_A))
    {
        player->position -= glm::cross(cameraDir, player->camera->cameraUp) * playerSpeed * deltaTime;
        player->camera->position -= glm::cross(cameraDir, player->camera->cameraUp) * playerSpeed * deltaTime;;
        player->playerModel->position -= glm::cross(cameraDir, player->camera->cameraUp) * playerSpeed * deltaTime;;

    }

    if (this->kbData.isKeyPressed(KeyboardData::KEY_D))
    {
        player->position += glm::cross(cameraDir, player->camera->cameraUp) * playerSpeed * deltaTime;;
        player->camera->position += glm::cross(cameraDir, player->camera->cameraUp) * playerSpeed * deltaTime;;
        player->playerModel->position += glm::cross(cameraDir, player->camera->cameraUp) * playerSpeed * deltaTime;;
    }

    if (this->kbData.isKeyPressed(KeyboardData::KEY_SPACE))
    {
        player->velocity.y = 2.0f;
        player->isGrounded = false;
    }

    this->kbData.isLocked = false;
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

void Player::onKbInput(GLFWwindow* window)
{


}

void Player::draw() {
   playerModel->draw();
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

   for (auto& chunk : this->world->loadedChunks)
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

Player::Player()
{
   playerModel = new Cube();
   camera = new Camera();

   position = glm::vec3(0.0f, 3.0f, 0.0f);
   camera->position = position;
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
   this->playerModel->position += transofrmation;
   this->camera->position += transofrmation;
   this->playerModel->position += transofrmation;
}
