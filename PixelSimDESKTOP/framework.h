#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include <iostream>
#include <thread>
#include <iomanip>
#include <vector>
#include <chrono>
#include <algorithm>

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

	// Internal Functions
	void createTexture(int id);
	void reloadTextures();
	void updateTexture(int id);
	void mouseDraw();

	inline bool running() { return applicationRunning; }

private:
	interfaceData data;
	bool applicationRunning = false;

	Game* game = nullptr;
	Interface* interface = nullptr;
	SDL_Window* window = nullptr;
	SDL_GLContext gl_context = nullptr;
};