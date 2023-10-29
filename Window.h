#pragma once
#include "Utils.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Game.h"

extern Game* game;

class GameWindowProxy {
private:
   static inline GLFWwindow* mWindow = nullptr;
public:
   static inline int mWindowHeight = 1920;
   static inline int mWindowWidth = 1080;
   static inline KeyboardData kbData;
   static void setName(std::string windowName) {};
   static void mouseCallback(GLFWwindow* window, double xpos, double ypos)
   {
      game->onMouseMove(window, xpos, ypos);
   }
   static void mouseClickCallback(GLFWwindow* window, int button, int action, int mods)
   {
      game->onMouseClick(window, button, action, mods);
   }
   static void kbCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
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

      if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9)
      {
         game->blockToPlaceIdx = key - GLFW_KEY_1;
      }
   }
   static void setWindowPtr(GLFWwindow* windowPtr)
   {
      mWindow = windowPtr;
   }
   static GLFWwindow* getWindowPtr()
   {
      return mWindow;
   }
   static void setupProxy()
   {
      glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GLFW_TRUE);
      glfwSetCursorPosCallback(mWindow, &GameWindowProxy::mouseCallback);
      glfwSetMouseButtonCallback(mWindow, &GameWindowProxy::mouseClickCallback);
      glfwSetKeyCallback(mWindow, &GameWindowProxy::kbCallback);
      glfwMakeContextCurrent(mWindow);
      //glfwSetFramebufferSizeCallback(mWindow, windowSizeChangedCallback);
   }
   static inline bool shouldWindowClose()
   {
      return glfwWindowShouldClose(mWindow);
   }
};
