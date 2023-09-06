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

// make compiler happy
class Interface; 
class Game;

class Framework
{
public:
	Framework();		// Constructor
	~Framework();	// Deconstructor

	bool init(const char* title, int xpos, int ypos, int width, int height);

	bool running() { return isRunning; }
	void handleEvents();
	void update();
	void render();
	void clean();
	void createTexture();
	void updateTexture();
	//inline void updateTexture() { glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixelData.data()); }

private:
	bool isRunning = false;
	bool hasSizeChanged = false;
	int texReloadedCount = 0;

	GLuint textureID = 255;
	int textureWidth = 0;
	int textureHeight = 0;
	std::vector<GLubyte> pixelData = std::vector<GLubyte>(9999 * 9999, 255); // declare to make big enough 

	Game* game = nullptr;
	Interface* interface = nullptr;
	SDL_Window* window = nullptr;
	SDL_GLContext gl_context = nullptr;
};