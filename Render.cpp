#include "Render.h"

void UiRenderer::renderHearts()
{
   for (size_t i = 0; i < 8; i++)
   {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, heartPositionStart + heartPositionJump * glm::vec3(i));
      model = glm::scale(model, glm::vec3(heartSize));

      basicUiShader->setMat4("model", model);
      basicUiShader->setInt("uiTexture", 4);
      glDrawArrays(GL_TRIANGLES, 0, 6);
   }
}

void UiRenderer::renderItemSelector()
{
   glm::mat4 model = glm::mat4(1.0f);
   model = glm::translate(model, selectorPosition);
   model = glm::scale(model, selectorSize);

   basicUiShader->setMat4("model", model);
   basicUiShader->setInt("uiTexture", 5);
   glDrawArrays(GL_TRIANGLES, 0, 6);

   auto currentlySelectedIndex = game->blockToPlaceIdx;
   glm::mat4 selectBorder = glm::mat4(1.0f);
   glm::vec3 position = glm::vec3(660 + 87 * currentlySelectedIndex, selectorPosition.y, 0);


   selectBorder = glm::translate(selectBorder, position);
   selectBorder = glm::scale(selectBorder, glm::vec3(72.0f, 50.0f, 1.0f));

   basicUiShader->setMat4("model", selectBorder);
   basicUiShader->setInt("uiTexture", 8);
   glDrawArrays(GL_TRIANGLES, 0, 6);

}



void UiRenderer::init()
{
   basicUiShader = std::make_unique<Shader>("uiVertexShader.hlsl", "uiPixelShader.hlsl");
   miniatureShader = std::make_unique<Shader>("minatureShader.hlsl", "minatureShader_frag.hlsl");
}

void UiRenderer::renderMiniature()
{
   glBindVertexArray(Cube::VAO);
   miniatureShader->use();
   glm::mat4 model = glm::mat4(1.0f);
   glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)GameWindowProxy::mWindowWidth / (float)GameWindowProxy::mWindowHeight, 0.1f, 3.0f);

   model = glm::translate(model, glm::vec3(1.2, -0.6, -0.6));
   model = glm::scale(model, glm::vec3(0.6));
   //model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1, 0, 0));

   miniatureShader->setMat4("model", model);
   miniatureShader->setMat4("projection", projection);
   miniatureShader->setInt("blockType", game->blockToPlaceIdx);
   glDrawArrays(GL_TRIANGLES, 0, 36);
}

void BlockRenderer::renderTransparent()
{
}

void BlockRenderer::renderNonTransparent()
{
}

void BlockRenderer::renderMiniature()
{

}

unsigned int TextureLoader::loadSkyBox(const std::vector<std::string>& faces, const unsigned int& whatTexture)
{
    //can be optimized
    unsigned int textureID;
    glGenTextures(1, &textureID);
    //glActiveTexture(whatTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
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

unsigned int TextureLoader::loadTexture(const std::string& path, const unsigned int& whatTexture)
{
   unsigned int texture;
   glGenTextures(1, &texture);
   glActiveTexture(whatTexture);
   glBindTexture(GL_TEXTURE_2D, texture);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


   int width, height, nrChannels;
   unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
   if (data)
   {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
      stbi_image_free(data);
   }
   else
   {
      std::string exceptionText = "Failed to load texture at" + path;
      throw std::runtime_error(exceptionText);
   }

   return texture;
}

void Renderer::prepareGlfwAndWindow()
{
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);

   GameWindowProxy::mWindowWidth = 1920;
   GameWindowProxy::mWindowHeight = 1080;

   GameWindowProxy::setWindowPtr(
      glfwCreateWindow(GameWindowProxy::mWindowWidth, GameWindowProxy::mWindowHeight, "MineMinecraft", nullptr, nullptr)
   );
   GameWindowProxy::setupProxy();

   if (GameWindowProxy::getWindowPtr() == nullptr)
   {
      throw new std::runtime_error("Failed to instantiate glfw window");
      exit(1);
   }

   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
   {
      throw new std::runtime_error("Failed to load GLAD");
      exit(1);
   }



   UiRectangle::init();
   Skybox::init();
   UiRenderer::init();
   Cube::init();

   loadTextures();

   glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureId);
   game = new Game(GameWindowProxy::kbData);

   skyboxShader = std::make_unique<Shader>("skybox.hlsl", "skybox_frag.hlsl");
   skyboxShader->use();
   skyboxShader->setInt("skybox", skyboxTextureId);

   game->blockShader->use();
   game->blockShader->setInt("texture1", 0);

}

void Renderer::loadTextures()
{
   TextureLoader::loadTexture("res/tp.png", GL_TEXTURE0);
   stbi_set_flip_vertically_on_load(true);
   TextureLoader::loadTexture("res/heart.png", GL_TEXTURE4);
   stbi_set_flip_vertically_on_load(false);
   TextureLoader::loadTexture("res/menuitem.png", GL_TEXTURE5);
   std::vector<std::string> faces = {
      "res/blank.png",
      "res/blank.png",
      "res/top.png",
      "res/bottom.png",
      "res/blank.png",
      "res/blank.png"
   };
   skyboxTextureId = TextureLoader::loadSkyBox(faces, GL_TEXTURE6);
   std::vector<std::string> faces2(6, "res/stars.png");
   skyboxStarsTextureId = TextureLoader::loadSkyBox(faces2, GL_TEXTURE7);
   TextureLoader::loadTexture("res/select.png", GL_TEXTURE8);

}

void Renderer::enterRenderLoop()
{
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   while (!GameWindowProxy::shouldWindowClose())
   {

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(game->skyColor.x, game->skyColor.y, game->skyColor.z, 1.0f);

      game->blockShader->use();
      glm::mat4 view = glm::lookAt(
         game->player->camera->position,
         game->player->camera->position + game->player->camera->front,
         game->player->camera->cameraUp
      );
      glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)GameWindowProxy::mWindowWidth / (float)GameWindowProxy::mWindowHeight, 0.1f, 200.0f);

      game->blockShader->setMat4("view", view);
      game->blockShader->setMat4("projection", projection);
      game->blockShader->setInt("texture1", 0);

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

      glm::mat4 aView = view;
      glm::mat4 someView;

      skyboxShader->use();

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxStarsTextureId);
      skyboxShader->setInt("skyboxStars", 1);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureId);
      skyboxShader->setInt("skyboxSunMoon", 2);

      glm::mat4 rot = glm::mat4(1.0f);
      rot = glm::rotate(rot, game->lastTime * TIME_SPEED_FACTOR, glm::vec3(1, 0, 0));
      someView = glm::mat4(glm::mat3(aView));
      skyboxShader->setMat4("view", someView);
      skyboxShader->setMat4("projection", projection);
      skyboxShader->setMat4("model", rot);
      skyboxShader->setFloat("starsLighting", cos(game->lastTime * TIME_SPEED_FACTOR + 3.14));

      glBindVertexArray(Skybox::VAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      glDepthFunc(GL_ALWAYS);

      game->crosshairShader->use();
      game->crosshairShader->setFloat("width", (float)GameWindowProxy::mWindowWidth);
      game->crosshairShader->setFloat("height", (float)GameWindowProxy::mWindowHeight);
      game->crosshair->draw();

      UiRectangle::use();
      UiRenderer::basicUiShader->use();
      auto ortho = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f);
      UiRenderer::basicUiShader->setMat4("projection", ortho);
      UiRenderer::renderHearts();
      UiRenderer::renderItemSelector();
      UiRenderer::renderMiniature();

      glBindVertexArray(0);
      glDepthFunc(GL_LESS);

      float currentFrame = glfwGetTime();
      deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;

      glFlush();
      glfwPollEvents();
   }
}
