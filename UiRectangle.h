#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class UiRectangle
{
public:
   static inline unsigned int VAO = 0;
   static inline unsigned int VBO = 0;
   static void init()
   {

      float vertices[] = {
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // bottom left

        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  // top left 
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // top right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // bottom left
      };

      glGenVertexArrays(1, &VAO);
      glGenBuffers(1, &VBO);

      glBindVertexArray(VAO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

      glBindVertexArray(0);
   }
   static void use()
   {
      glBindVertexArray(VAO);
   }

};

