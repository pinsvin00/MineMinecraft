// MineKraft.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Game.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#include <glm/gtx/euler_angles.hpp>
#include "Cube.h"
#include <conio.h>
#include "Crosshair.h"
#include "m_math.h"
#include "Skybox.h"

//global variables

int _w = 800, _h = 600;
bool gameShouldClose = false;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

unsigned int Cube::VAO = 0;
unsigned int Cube::VBO = 0;
unsigned int Crosshair::VAO = 0;
unsigned int Crosshair::VBO = 0;
unsigned int skyboxTextureId = 0;

Game* game = nullptr;
std::unique_ptr<Shader> skyboxShader = nullptr;
std::unique_ptr<Skybox> skybox = nullptr;

GLFWwindow* window = nullptr;
KeyboardData kbData;

//main functions

void windowSizeChangedCallback(GLFWwindow* window, int w, int h)
{
   _w = w;
   _h = h;
   glViewport(0, 0, w, h);
}


void kbCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W)
    {
        kbData.setKeyPressed(KeyboardData::KEY_W, action != GLFW_RELEASE);
    }
    if (key == GLFW_KEY_S)
    {
        kbData.setKeyPressed(KeyboardData::KEY_S, action != GLFW_RELEASE);
    }

    if (key == GLFW_KEY_A)
    {
        kbData.setKeyPressed(KeyboardData::KEY_A, action != GLFW_RELEASE);
    }

    if (key == GLFW_KEY_D)
    {
        kbData.setKeyPressed(KeyboardData::KEY_D, action != GLFW_RELEASE);
    }

    if (key == GLFW_KEY_SPACE)
    {
        kbData.setKeyPressed(KeyboardData::KEY_SPACE, action != GLFW_RELEASE);
    }

    if (key >= GLFW_KEY_1 &&  key <= GLFW_KEY_9)
    {
       game->player->blockToPlaceIdx = key - GLFW_KEY_1;
    }
}

void runGameLoop()
{
    while (!gameShouldClose)
    {
        game->processGame();
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
   game->onMouseMove(window, xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
   game->onMouseClick(window, button, action, mods);
}


void loggerLoop()
{
    while (true)
    {
        //The delta time is in seconds so we're using 1.0 instead of 1000
        //std::cout << "FPS : " << std::floor(1.0 / deltaTime) << std::endl;
        //std::cout << "Game updates/sec : " << std::floor(1.0 / game->deltaTime) << std::endl;
        //auto position = game->player->position;
        //std::cout << "Player position : " << " X:" << position.x << " Y:" << position.y << " Z:" << position.z << std::endl;
        //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        //system("cls");
    }



}

unsigned int loadSkyBox()
{
   std::vector<std::string> faces = {
        "res/right.jpg",
        "res/left.jpg",
        "res/top.jpg",
        "res/bottom.jpg",
        "res/front.jpg",
        "res/back.jpg"
   };

   unsigned int textureID;
   glGenTextures(1, &textureID);
   glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

   int width, height, nrChannels;
   for (unsigned int i = 0; i < faces.size(); i++)
   {
      unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
      if (data)
      {
         glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
         );
         stbi_image_free(data);
      }
      else
      {
         std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
         stbi_image_free(data);
      }
   }
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

   return textureID;
}

void render()
{
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window))
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(game->skyColor.x, game->skyColor.y, game->skyColor.z, 1.0f);
        game->blockShader->use();

        glm::mat4 view = glm::lookAt(
           game->player->camera->position,
           game->player->camera->position + game->player->camera->front,
           game->player->camera->cameraUp
        );
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)_w / (float)_h, 0.1f, 200.0f);

        game->blockShader->setMat4("view", view);
        game->blockShader->setMat4("projection", projection);

        game->blockShader->setVec3("light.ambient", glm::vec3(0.3f));
        game->blockShader->setVec3("light.diffuse", glm::vec3(game->lightIntensity));
        game->blockShader->setVec3("light.direction", game->lightDirection);

        glBindVertexArray(Chunk::chunkVAO);
        for (size_t i = 0; i < game->world->loadedChunks->size(); i++)
        {
            game->blockShader->setInt("outlinedCubeIdx", -1);
            auto chunk = game->world->loadedChunks->at(i);
            if (game->outlinedCube && chunk == game->outlinedCube->cubesChunk)
            {
               game->blockShader->setInt("outlinedCubeIdx", game->outlinedCube->inChunkIdx);
            }
            chunk->render();
        }

        for (size_t i = 0; i < game->world->loadedChunks->size(); i++)
        {
           auto chunk = game->world->loadedChunks->at(i);
           chunk->renderTransparent();
        }

        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader->use();
        glm::mat4 aView = view;
        glm::mat4 someView = glm::mat4(glm::mat3(aView));
        skyboxShader->setMat4("view", someView);
        skyboxShader->setMat4("projection", projection);
        skyboxShader->setInt("skybox", 0);
        // skybox cube
        glBindVertexArray(Skybox::VAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 1);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default


        //game->crosshairShader->use();
        //game->crosshairShader->setFloat("width", (float)_w);
        //game->crosshairShader->setFloat("height", (float)_h);

        //game->crosshair->draw();

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glFlush();
        glfwPollEvents();
    }
}




int main()
{

   //WINDOW RELATED STUFF

   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);

   _w = 1920;
   _h = 1080;

   window = glfwCreateWindow(_w, _h, "MineMinecraft", nullptr, nullptr);
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

   glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
   glfwSetCursorPosCallback(window, &mouse_callback);
   glfwSetMouseButtonCallback(window, &mouse_button_callback);
   glfwSetKeyCallback(window, &kbCallback);

   if (window == nullptr)
   {
      std::cout << "Failed to create GLFW window" << std::endl;
      exit(1);
   }

   glfwMakeContextCurrent(window);
   glfwSetFramebufferSizeCallback(window, windowSizeChangedCallback);


   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
   {
      std::cout << "Failed to load GLAD" << std::endl;
      exit(1);
   }
   //opengl startup finished

   skyboxTextureId = loadSkyBox();
   Skybox::init();
   glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureId);
   game = new Game(kbData);


   //SHADER RELATED STUFF
   double glTime = 0.0;
   double greenValue = 0.0;

   skyboxShader = std::make_unique<Shader>("skybox.hlsl", "skybox_frag.hlsl");
   skyboxShader->use();
   skyboxShader->setInt("skybox", skyboxTextureId);

   std::vector<std::string> data = {
      "res/tp.png",
   };
   std::vector<int> textures;


   for (size_t i = 0; i < data.size(); i++)
   {
      auto record = data[i];

      unsigned int texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


      int width, height, nrChannels;
      unsigned char* data = stbi_load(record.c_str(), &width, &height, &nrChannels, 0);
      if (data)
      {
         glActiveTexture(GL_TEXTURE0);
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
         glGenerateMipmap(GL_TEXTURE_2D);
         stbi_image_free(data);
      }
      else
      {
         std::cout << "Failed to load texture" << std::endl;
         return -1;
      }
      textures.push_back(texture);

   }


   game->blockShader->use();
   game->blockShader->setInt("texture1", 0);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, textures[0]);


   //Vertices for square with textures


   std::thread game_thread(runGameLoop);
   std::thread logger_thread(loggerLoop);
   game_thread.detach();
   logger_thread.detach();

   render();

   gameShouldClose = true;


   glfwTerminate();
   return 0;
}