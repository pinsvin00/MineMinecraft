#pragma once


#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern float deltaTime;

class Camera {
public:
   float yaw = 0.0f;
   float pitch = 0.0f;
   float lastX = 400, lastY = 300;
   bool firstMouse = true;
   glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
   glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
   glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
   bool isGrounded = true;

   void mouse_callback(GLFWwindow* window, double xpos, double ypos)
   {
      if (firstMouse)
      {
         lastX = xpos;
         lastY = ypos;
         firstMouse = false;
      }

      float xoffset = xpos - lastX;
      float yoffset = lastY - ypos;
      lastX = xpos;
      lastY = ypos;

      float sensitivity = 0.1f;
      xoffset *= sensitivity;
      yoffset *= sensitivity;

      yaw += xoffset;
      pitch += yoffset;

      if (pitch > 89.0f)
         pitch = 89.0f;
      if (pitch < -89.0f)
         pitch = -89.0f;

      glm::vec3 direction;
      direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
      direction.y = sin(glm::radians(pitch));
      direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
      front = glm::normalize(direction);
   }
};


