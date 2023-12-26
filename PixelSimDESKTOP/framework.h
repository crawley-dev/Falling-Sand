#pragma once
#include "pch.h"
#include "interface.h"
#include "game.h"

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
	bool applicationRunning = false;

	interfaceData data;
	Game* game				 = nullptr; // std::unique_ptr<Game>      
	Interface* interface	 = nullptr;	// std::unique_ptr<Interface>  
	SDL_Window* window		 = nullptr; // could use a unique ptr but would require a refactor, 
	SDL_GLContext gl_context = nullptr; // thats too much effort for some memory management.
};