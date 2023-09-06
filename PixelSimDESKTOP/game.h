#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <vector>

#include "pixel.h"

class Game
{
public:
	Game();
	~Game();

	void init(std::vector<GLubyte> pixelData, int textureW, int textureH);
	void createPixel(int range, bool flag, int texIdx, int x, int y, PixelType type);
	void updatePixel(int x, int y, int r, int g, int b, int a);
	
	inline int texIdx(int x, int y) { return 4 * (y * texW + x); }
	inline std::vector<GLubyte> getTextureData() { return textureData; }

private:
	int texW, texH;
	std::vector<Pixel> pixels;
	std::vector<GLubyte> textureData;
	PixelType Types[4];
	PixelType AIR, SAND, WATER, ROCK;
};