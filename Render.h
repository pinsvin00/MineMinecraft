#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Utils.h"
#include "Game.h"
#include "Skybox.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "UiRectangle.h"
#include "stb_image.h"



extern Game* game;

class TextureLoader {
private:
   static inline bool flipImage = false;

public:
   static void setFlipImage(bool flip)
   {
      flipImage = flip;
   }
   static unsigned int loadSkyBox(const std::vector<std::string>& faces, const unsigned int& whatTexture);
   static unsigned int loadTexture(const std::string& path, const unsigned int& whatTexture);
};
class BlockRenderer {
   static void renderTransparent();
   static void renderNonTransparent();
   static void renderMiniature();
};

class UiRenderer {

   static const inline glm::vec3 selectorSize = glm::vec3(720.0f, 60.0f, 1.0f);
   static const inline glm::vec3 selectorPosition = glm::vec3(640.0f + 320.0f, 120.0f, 0.0f);
   static const inline glm::vec3 heartPositionStart = glm::vec3(640.0f, selectorPosition.y + 50.0f, 0.0f);
   static const inline glm::vec3 heartSize = glm::vec3(24.0f, 24.0f, 1.0f);
   static const inline glm::vec3 heartPositionJump = glm::vec3(heartSize.x + 5.0f, 0, 0);
   static inline std::unique_ptr<Shader> miniatureShader = nullptr;
public:
   static inline std::unique_ptr<Shader> blockShader = nullptr;
   static inline std::unique_ptr<Shader> basicUiShader = nullptr;
   static void renderHearts();
   static void renderItemSelector();
   static void init();
   static void renderMiniature();
};


class Renderer {
private:
   static inline std::unique_ptr<Shader> skyboxShader = nullptr;
   static inline std::unique_ptr<Skybox> skybox = nullptr;

   static inline unsigned int skyboxTextureId = 0;
   static inline unsigned int skyboxStarsTextureId = 0;
   static inline float deltaTime = 0.0f;	// Time between current frame and last frame
   static inline float lastFrame = 0.0f; // Time of last frame
   
   //other renderers
   static inline BlockRenderer blockRenderer;
   static inline UiRenderer uiRenderer;

public:
   static void prepareGlfwAndWindow();
   static void loadTextures();
   static void enterRenderLoop();

};
