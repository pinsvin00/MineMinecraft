#include "Crosshair.h"

void Crosshair::init()
{
   float vertices[] = {
      // first triangle
      0.5f,  0.5f, 0.0f,  // top right
      0.5f, -0.5f, 0.0f,  // bottom right
      -0.5f,  0.5f, 0.0f,  // top left 
      // second triangle
      0.5f, -0.5f, 0.0f,  // bottom right
      -0.5f, -0.5f, 0.0f,  // bottom left
      -0.5f,  0.5f, 0.0f   // top left
   };

   glGenBuffers(1, &VBO);
   glGenVertexArrays(1, &VAO);

   glBindVertexArray(VBO);
   glBindBuffer(GL_ARRAY_BUFFER, VAO);

   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);
}

void Crosshair::draw()
{
   glDrawArrays(GL_TRIANGLES, 0, 6);
}
