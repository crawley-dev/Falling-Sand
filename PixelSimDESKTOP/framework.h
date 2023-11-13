#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>	
#include <OpenImageIO/imageio.h> // OpenImageIO

#include <iostream>
#include <thread>
#include <iomanip>
#include <vector>
#include <chrono>
#include <algorithm>
#include <memory>

#include "interfaceData.h"

// make compiler happy
class Interface; 
class Game;

class Framework
{
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
	void loadImageRGB(std::string path, int textureIndex);
	void loadImageRGBA(std::string path, int textureIndex); // png is somehow rgb not rgba..
	void createTexture(int textureIndex);
	void reloadTextures();
	void updateTexture(int textureIndex);

	// Internal Functions	
	void mouseDraw();
	inline bool running() { return applicationRunning; }

private:
	interfaceData data;
	bool applicationRunning = false;

	/*std::unique_ptr<Game>     */ Game*	  game = nullptr;
	/*std::unique_ptr<Interface>*/ Interface* interface = nullptr;
	SDL_Window* window = nullptr;
	SDL_GLContext gl_context = nullptr;
};