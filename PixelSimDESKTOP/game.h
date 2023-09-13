#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <vector>

#include "cell.h"

class Game
{
public:
	Game();
	~Game();

	void init(std::vector<GLubyte> pixelData, int textureW, int textureH);
	void reload(std::vector<GLubyte> pixelData, int textureW, int textureH);
	void update();
	void cellUpdate(Cell& p);

	void createCell(int range, bool flag, int texIdx, int x, int y, int PixelTypeID);
	void updatePixel(int x, int y, int r, int g, int b, int a);
	
	CellType varyPixelColour(int range, int PixelTypeID);
	void changeCellType(int x, int y, int PixelTypeID);
	void mouseDraw(int x, int y, int radius, int PixelTypeID);

	void swapCells(int x1, int y1, int x2, int y2);
	void swapCells(Cell& p1, Cell& p2);
	bool checkDensity(int x1, int y1, int x2, int y2);
	bool checkDensity(Cell& p1, int delX, int delY);


	inline bool outOfBounds(int x, int y) { return (x > texW || x < 0 || y > texH || y < 0); }
	inline int cellIdx(int x, int y) { return y * texW + x; }
	inline int texIdx(int x, int y) { return 4 * (y * texW + x); }
	inline std::vector<GLubyte> getTextureData() { return textureData; }

private:
	int texW, texH;
	std::vector<Cell> cells;
	std::vector<GLubyte> textureData;
	CellType Types[4];
	CellType AIR, SAND, WATER, ROCK;
};