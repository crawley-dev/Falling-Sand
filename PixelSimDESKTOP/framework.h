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
	Framework();		// Constructor
	~Framework();	// Deconstructor

	// Application Abstraction Functions
	bool init(const char* title, int xpos, int ypos, int width, int height);
	void handleEvents();
	void update();
	void render();
	void clean();

	// Internal Functions
	void createTexture();
	void updateTexture();
	void mouseDraw();

	inline bool running() { return applicationRunning; }
	//inline void updateTexture() { glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixelData.data()); }

private:
	interfaceData data;
	bool applicationRunning = false;
	int framesSinceReload = 0;

	/*
	bool runSim = true;
	bool hasSizeChanged = false;
	int texReloadedCount = 0;
	int cellDrawType = 1; // clDrawType
	int cellDrawSize = 40; // clDrawSize
	
	int textureWidth = 0;
	int textureHeight = 0;
	GLuint textureID = 255;
	*/

	std::vector<GLubyte> textureData; // declare to make big enough 

	Game* game = nullptr;
	Interface* interface = nullptr;
	SDL_Window* window = nullptr;
	SDL_GLContext gl_context = nullptr;
};