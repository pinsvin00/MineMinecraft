#include <iostream>
#include <thread>
#include "Shader.h"


#include "UiRectangle.h"
#include "Utils.h"
#include "Window.h"
#include "Render.h"
#include "Game.h"

#include <vector>
#include "Cube.h"
#include <conio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//global variables

int _w = 800, _h = 600;
bool gameShouldClose = false;

unsigned int Cube::VAO = 0;
unsigned int Cube::VBO = 0;
unsigned int Crosshair::VAO = 0;
unsigned int Crosshair::VBO = 0;



Game* game = nullptr;


void runGameLoop()
{
    while (!gameShouldClose)
    {
        game->processGame();
    }
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

int main()
{

   Renderer::prepareGlfwAndWindow();

   std::thread game_thread(runGameLoop);
   std::thread logger_thread(loggerLoop);
   game_thread.detach();
   logger_thread.detach();

   Renderer::enterRenderLoop();

   gameShouldClose = true;

   glfwTerminate();
   return 0;
}