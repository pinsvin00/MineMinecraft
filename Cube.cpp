#include "Cube.h"

bool Cube::checkCollision(Cube* c)
{
   auto myVerts = this->getVertices();
   auto theirVerts = c->getVertices();

   auto m_vecMin_1 = myVerts[LEFT_DOWN_1];
   auto m_vecMax_1 = myVerts[RIGHT_UP_2];
   auto m_vecMin_2 = theirVerts[LEFT_DOWN_1];
   auto m_vecMax_2 = theirVerts[RIGHT_UP_2];


   return(
      m_vecMax_1.x > m_vecMin_2.x &&
      m_vecMin_1.x < m_vecMax_2.x&&
      m_vecMax_1.y > m_vecMin_2.y &&
      m_vecMin_1.y < m_vecMax_2.y&&
      m_vecMax_1.z > m_vecMin_2.z &&
      m_vecMin_1.z < m_vecMax_2.z
      );
}

Box3 Cube::getCollider()
{
   auto myVerts = this->getVertices();
   auto m_vecMin_1 = myVerts[LEFT_DOWN_1];
   auto m_vecMax_1 = myVerts[RIGHT_UP_2];
   return Box3(m_vecMin_1, m_vecMax_1);
}

std::vector<glm::vec3> Cube::getVertices()
{
   std::vector<glm::vec3> v;
   for (auto element : verts)
   {
      element += position;
      v.push_back(element);
   }

   return v;
}

void Cube::processMat()
{
   model = glm::mat4(1.0f);
   model = glm::translate(model, position);
   model = glm::scale(model, scale);
   shader->setMat4("model", model);
}

void Cube::draw()
{
   processMat();
   shader->setMat4("model", model);

   for (size_t i = 0; i < 6; i++)
   {
      auto res = facesToRender & 1 << i;
      if (res) {
         glDrawArrays(GL_TRIANGLES, i * 6, 6);
      }
   }

}

void Cube::init()
{
   //przednia, prawa, lewa, z tylu, na gorze, na dole
   const float cubeVertices[] = {
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, //tylnia
      0.5f, -0.5f, -0.5f,  0.1f, 0.0f,
      0.5f,  0.5f, -0.5f,  0.1f, 0.1f,
      0.5f,  0.5f, -0.5f,  0.1f, 0.1f,
      -0.5f,  0.5f, -0.5f,  0.0f, 0.1f,
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,


      -0.5f, -0.5f,  0.5f,  0.1f, 0.0f, //przednia
      0.5f, -0.5f,  0.5f,  0.2f, 0.0f,
      0.5f,  0.5f,  0.5f,  0.2f, 0.1f,
      0.5f,  0.5f,  0.5f,  0.2f, 0.1f,
      -0.5f,  0.5f,  0.5f,  0.1f, 0.1f,
      -0.5f, -0.5f,  0.5f,  0.1f, 0.0f,

      -0.5f,  0.5f,  0.5f,  0.3f, 0.0f, //lewa
      -0.5f,  0.5f, -0.5f,  0.3f, 0.1f,
      -0.5f, -0.5f, -0.5f,  0.2f, 0.1f,
      -0.5f, -0.5f, -0.5f,  0.2f, 0.1f,
      -0.5f, -0.5f,  0.5f,  0.2f, 0.0f,
      -0.5f,  0.5f,  0.5f,  0.3f, 0.0f,

      0.5f,  0.5f,  0.5f,  0.4f, 0.0f, //prawa
      0.5f,  0.5f, -0.5f,  0.4f, 0.1f,
      0.5f, -0.5f, -0.5f,  0.3f, 0.1f,
      0.5f, -0.5f, -0.5f,  0.3f, 0.1f,
      0.5f, -0.5f,  0.5f,  0.3f, 0.0f,
      0.5f,  0.5f,  0.5f,  0.4f, 0.0f,

      -0.5f, -0.5f, -0.5f,  0.4f, 0.1f, //dolna
      0.5f, -0.5f, -0.5f,  0.5f, 0.1f,
      0.5f, -0.5f,  0.5f,  0.5f, 0.0f,
      0.5f, -0.5f,  0.5f,  0.5f, 0.0f,
      -0.5f, -0.5f,  0.5f,  0.4f, 0.0f,
      -0.5f, -0.5f, -0.5f,  0.4f, 0.1f,


      -0.5f,  0.5f, -0.5f,  0.5f, 0.1f, //gorna
      0.5f,  0.5f, -0.5f,  0.6f, 0.1f,
      0.5f,  0.5f,  0.5f,  0.6f, 0.0f,
      0.5f,  0.5f,  0.5f,  0.6f, 0.0f,
      -0.5f,  0.5f,  0.5f,  0.5f, 0.0f,
      -0.5f,  0.5f, -0.5f,  0.5f, 0.1f

   };

   glGenBuffers(1, &VBO);
   glGenVertexArrays(1, &VAO);

   glBindVertexArray(VBO);
   glBindBuffer(GL_ARRAY_BUFFER, VAO);


   glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
   // 3. then set our vertex attributes pointers
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);
   // texture coord attribute
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);
}
