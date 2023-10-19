#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <vector>
#include <ranges>
#include <algorithm>
#include <stdexcept>


#include "cell.h"
#include "interfaceData.h"

class Game
{
public:
	Game();
	~Game();

	void init(std::vector<GLubyte> texData, int textureW, int textureH, int scale);
	void reload(std::vector<GLubyte> texData, int textureW, int textureH, int newScaleFactor);
	void reset(bool& resetSim);
	void update(interfaceData& data);

	void cellUpdate(Cell& p);
	void gameOfLifeUpdate(interfaceData& data);
	void updatePixel(Cell& c);
	
	CellType varyPixelColour(int range, int CellTypeID);
	void mouseDraw(int x, int y, int radius, int chance, int CellTypeID, int CellDrawShape, int range);

	void changeCellType(int x, int y, int CellTypeID, int range);
	void swapCells(Cell& p1, Cell& p2);
	bool checkDensity(Cell& p1, int delX, int delY);

	void drawCircle(int xc, int yc, int x, int y, int PixelTypeID, int range);

	void updateSand(Cell& c);
	void updateWater(Cell& c);
	
	inline bool outOfBounds(int x, int y) { return (x >= cellW || x < 0 || y >= cellH || y < 0); }
	inline int cellIdx(int x, int y) { return (y * cellW) + x; }
	inline int texIdx(int x, int y) { return 4 * (y * texW + x); }
	inline std::vector<GLubyte> getTextureData() { return textureData; }
	
	/* Main problem THIS DOESNT FIX: escaping a function that wants to do stuff on an invalid cell.
	* inline Cell& getCell(int x, int y) 
	*	if (outOfBounds(x,y) {
	* 		printf("out of bounds: (%d,%d)\n", x, y);
	* 		throw std::invalid_argument("out of vector range.");
	* 		//return cells[0];
	* 	}
	*	return cells[cellIdx(x, y)];
	* }
	*/

private:
	int texW, texH, cellScale, cellW, cellH;	
	std::vector<Cell> cells;
	std::vector<GLubyte> textureData;
	CellType Types[5];
	CellType EMPTY, SAND, WATER, CONCRETE, ALIVE;
};