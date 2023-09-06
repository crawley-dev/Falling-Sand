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
	void update();
	void calcNewPositions();

	void createPixel(int range, bool flag, int texIdx, int x, int y, int PixelTypeID);
	void updatePixel(int x, int y, int r, int g, int b, int a);
	
	PixelType varyPixelColour(int range, int PixelTypeID);
	void swapPixels(int x1, int y1, int x2, int y2);
	void changePixelType(int x, int y, int PixelTypeID);
	void mouseDraw(int x, int y, int radius, int PixelTypeID);

	inline int pixIdx(int x, int y) { return y * texW + x; }
	inline int texIdx(int x, int y) { return 4 * (y * texW + x); }
	inline std::vector<GLubyte> getTextureData() { return textureData; }

private:
	int texW, texH;
	std::vector<Pixel> pixels;
	std::vector<GLubyte> textureData;
	PixelType Types[4];
	PixelType AIR, SAND, WATER, ROCK;
};