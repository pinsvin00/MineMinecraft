// MineKraft.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
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
#include "Crosshair.h"
#include "m_math.h"

int _w = 800, _h = 600;

void windowSizeChangedCallback(GLFWwindow* window, int w, int h)
{
   _w = w;
   _h = h;
   glViewport(0, 0, w, h);
}

float cameraSpeed = 20.0f;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

unsigned int Cube::VAO = 0;
unsigned int Cube::VBO = 0;
unsigned int Crosshair::VAO = 0;
unsigned int Crosshair::VBO = 0;

Cube* cube1;

Game* game;

void processInput(GLFWwindow* window)
{
   game->onKbInput(window);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
   game->onMouseMove(window, xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
   game->onMouseClick(window, button, action, mods);
}




int main()
{

   //WINDOW RELATED STUFF

   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   _w = 1280;
   _h = 720;

   GLFWwindow* window = glfwCreateWindow(_w, _h, "My app", nullptr, nullptr);
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


   glfwSetCursorPosCallback(window, &mouse_callback);
   glfwSetMouseButtonCallback(window, &mouse_button_callback);
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

   game = new Game();


   //SHADER RELATED STUFF
   double glTime = 0.0;
   double greenValue = 0.0;


   std::vector<std::string> data = {
      "res/tp.jpg",
      "res/face.jpg",
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
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


      int width, height, nrChannels;
      unsigned char* data = stbi_load(record.c_str(), &width, &height, &nrChannels, 0);
      if (data)
      {
         glActiveTexture(GL_TEXTURE0);
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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
   game->blockShader->setInt("texture2", 1);




   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, textures[0]);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, textures[1]);


   //Vertices for square with textures
   glEnable(GL_DEPTH_TEST);


   glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);


   cube1 = new Cube();
   cube1->shader = game->blockShader;
   cube1->position = glm::vec3(0.0f, 5.0f, 0.0f);

   while (!glfwWindowShouldClose(window))
   {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(0.3f, 0.4f, 0.7f, 1.0f);

      processInput(window);
      auto time = (float)glfwGetTime();
      game->blockShader->use();

      glm::mat4 model = glm::mat4(1.0f);
      glm::mat4 view = glm::mat4(1.0f);
      glm::mat4 projection = glm::mat4(1.0f);

      view = glm::lookAt(
         game->player->camera->position,
         game->player->camera->position + game->player->camera->front,
         game->player->camera->cameraUp
      );

      projection = glm::perspective(glm::radians(90.0f), (float)_w / (float)_h, 0.1f, 200.0f);

      game->blockShader->setMat4("view", view);
      game->blockShader->setMat4("projection", projection);


      cube1->draw();
      cube1->facesToRender = 0b111111;

      for (size_t i = 0; i < game->world->loadedChunks.size(); i++)
      {
         auto chunk = game->world->loadedChunks[i];
         for (size_t i = 0; i < chunk->cubes.size(); i++)
         {
            auto cube = chunk->cubes[i];
            if (!cube->dontDraw)
            {
               cube->shader = game->blockShader;
               cube->draw();
            }
         }
      }



      game->crosshairShader->use();
      game->crosshairShader->setFloat("width", _w);
      game->crosshairShader->setFloat("height", _h);

      game->crosshair->draw();

      float currentFrame = glfwGetTime();
      game->deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;

      game->processGame();
      glfwSwapBuffers(window);
      glfwPollEvents();
   }


   glfwTerminate();

   return 0;
}