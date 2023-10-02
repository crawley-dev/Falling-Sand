#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <vector>
#include <ranges>

#include "cell.h"
#include "interfaceData.h"

class Game
{
public:
	Game();
	~Game();

	void init(std::vector<GLubyte> textureData, int textureWidth, int textureHeight, int scale);
	void reload(std::vector<GLubyte> textureData, int textureWidth, int textureHeight, int newScaleFactor);
	void reset(int cellTypeID, bool& resetSim);
	void update(interfaceData& data);
	void cellUpdate(Cell& p);

	void createCell(int range, bool flag, int textureIdx, int x, int y, int cellTypeID);
	void updatePixel(int x, int y, int r, int g, int b, int a);
	void updatePixel(Cell& c);
	
	CellType varyPixelColour(int range, int cellTypeID);
	void mouseDraw(int x, int y, int radius, int chance, int cellTypeID, int cellDrawShape, int range);

	void changeCellType(int x, int y, int cellTypeID, int range);
	void swapCells(Cell& p1, Cell& p2);
	bool checkDensity(Cell& p1, int deltaX, int deltaY);

	void updateSand(Cell& c);
	void updateWater(Cell& c);

	inline bool outOfBounds(int x, int y) { return (x >= cellWidth || x < 0 || y >= cellHeight || y < 0); }
	inline int cellIdx(int x, int y) { return (y * cellWidth) + x; }
	inline int texIdx(int x, int y) { return 4 * (y * textureWidth + x); }
	inline std::vector<GLubyte> getTextureData() { return textureData; }

private:
	int textureWidth, textureHeight, cellScale, cellWidth, cellHeight;	
	std::vector<Cell> cells;
	std::vector<GLubyte> textureData;
	CellType Types[4];
	CellType EMPTY, SAND, WATER, CONCRETE;
};