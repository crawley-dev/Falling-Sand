#pragma once
#include "pch.h"
//#include "imgui.h"
//#include "imgui_impl_sdl2.h"
//#include "imgui_impl_opengl3.h"
//#include <SDL.h>
//#include <SDL_opengl.h>
//#include <SDL_image.h>
#include "interface.h"
#include "game.h"


using namespace PixelPhysics;
class Framework {
public:
    Framework();	// Constructor
    ~Framework();	// Deconstructor

    // Application Cycle Abstraction
    bool init(const char* title, int xpos, int ypos, int width, int height);
    void handleEvents();
    void update();
    void render();
    void clean();

    // Texture Handling
    void loadImageRGB(TextureData& texture, std::string path);
    void loadImageRGBA(TextureData& texture, std::string path); // png is somehow rgb not rgba..

    void saveToFile(TextureData& texture);
    void loadFromFile(TextureData& texture, std::string path);

    void createTexture(TextureData& texture);
    void updateTexture(TextureData& texture);
    void reloadTextures();

    // Internal Functions
    void mouseDraw();
    inline bool running() const { return applicationRunning; } // const means this function cannot modify any member variables

private:
    bool applicationRunning = false;

    AppState state;
    Game* game = nullptr; // std::unique_ptr<Game>      
    Interface* interface = nullptr;	// std::unique_ptr<Interface>  
    SDL_Window* window = nullptr; // could use a unique ptr but would require a refactor, 
    SDL_GLContext gl_context = nullptr; // thats too much effort for some memory management.
};
