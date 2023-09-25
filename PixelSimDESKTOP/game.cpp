#pragma once

#include "game.h"

Game::Game() {}
Game::~Game() {}

// annoying that texture --> tex for w,h but not data!!
void Game::init(std::vector<GLubyte> texData, int textureW, int textureH, int scale)
{
	cellScale   = scale;
	textureData = texData;
	texW        = textureW;
	texH        = textureH;
	cellW       = texW / cellScale;
	cellH       = texH / cellScale;

//	Types[0] = AIR	 = CellType(0, 188, 231, 255, 255, 0	);
//	Types[0] = AIR	 = CellType(0, 255, 255, 255, 255, 0	);
	Types[0] = EMPTY	= CellType(0,  44,  44,  44, 255,    0, 0);
	Types[1] = SAND		= CellType(1, 245, 215, 176, 255, 1600, 4);
	Types[2] = WATER	= CellType(2, 20 , 20 , 255, 125,  997, 4);
	Types[3] = CONCRETE = CellType(3, 200, 200, 200, 255, 2000, 4);
	

	cells.clear(); // once upon a time, it didn't reset cell arr on reload :/ oops

	for (int y = 0; y < cellH; ++y)
		for (int x = 0; x < cellW; ++x) {
			cells.push_back(Cell(cellIdx(x, y), x, y, 1, EMPTY, false));
			updatePixel(cells[cellIdx(x,y)]);
		}

	//printf("texureData Size: %d\n", (int)textureData.size());
	//printf("cells Size: %d\n", (int)cells.size());
	//printf("tex width: %d\n", texW);
	//printf("tex height: %d\n", texH);
	//printf("cell width: %d\n", cellW);
	//printf("cell height: %d\n", cellH);
	//printf("\n");
}

void Game::reload(std::vector<GLubyte> newTexData, int newTexW, int newTexH, int newScaleFactor)
{
	int newCellW = newTexW / newScaleFactor;
	int newCellH = newTexH / newScaleFactor;
	std::vector<Cell> newCells;

	for (int y = 0; y < newCellH; ++y) 
		for (int x = 0; x < newCellW; ++x) {
			if (outOfBounds(x, y)) 
				newCells.push_back(Cell(cellIdx(x, y), x, y, 1, EMPTY, false));
			else
				newCells.push_back(cells[cellIdx(x, y)]);
		}

	cellScale = newScaleFactor;
	textureData = newTexData;
	texW = newTexW;
	texH = newTexH;
	cellW = newCellW;
	cellH = newCellH;
	cells = newCells;
}

void Game::reset(bool& resetSim)
{
	cells.clear();

	for (int y = 0; y < cellH; ++y)
		for (int x = 0; x < cellW; ++x) {
			cells.push_back(Cell(cellIdx(x, y), x, y, 1, EMPTY, false));
			updatePixel(cells[cellIdx(x,y)]);
		}
	resetSim = false;
}

void Game::update(interfaceData& data)
{
	if (texH == 0 || texW == 0) return;
	//if (data.runSim) mouseDraw(cellW, cellH, (data.frame % cellH) -1 , 100, 0, 1, 0);
	//if (data.runSim) mouseDraw(cellW, cellH,       data.frame % cellH, 100, 1, 1, 0);

	if (data.doTopBot) {
		for (Cell& c : cells) {
			if (data.runSim) {
				cellUpdate(c);
				c.flag = false; // to this day (25.9.23) i don't know why i can do this. 
			}
			updatePixel(c);
		}
		//for (Cell & c : cells) c.flag = false;
	}
	else {
		for (int y = cellH - 1; y >= 0; --y)	
			for (int x = cellW - 1; x >= 0; --x) {
				Cell& c = cells[cellIdx(x,y)];
		
				if (data.runSim) cellUpdate(c);
				updatePixel(c);
				c.flag = false;
				if (data.runSim) cellUpdate(c); // flickers without this.. WAT
			}
	}
	if (data.runSim) data.frame++;
}

void Game::cellUpdate(Cell& c)
{
	if (c.flag || c.type.id == 0 || c.type.id == 3) return;
	else if (c.type.id == 1) updateSand(c);
	else if (c.type.id == 2) updateWater(c);
}


void Game::updatePixel(Cell& c)
{
	if (outOfBounds(c.x, c.y)) return;
	for (int tY = 0; tY < cellScale; ++tY)
		for (int tX = 0; tX < cellScale; ++tX) {
			int idx = texIdx((c.x * cellScale) + tX, (c.y * cellScale) + tY);
			textureData[idx + 0] = c.type.r;
			textureData[idx + 1] = c.type.g;
			textureData[idx + 2] = c.type.b;
			textureData[idx + 3] = c.type.a;
		}
}

CellType Game::varyPixelColour(int range, int PixelTypeID)
{
	if (range <= 0) return Types[PixelTypeID];
	CellType material = Types[PixelTypeID];
	material.r = material.r - rand() % range;
	material.g = material.g - rand() % range;
	material.b = material.b - rand() % range;
	material.a = material.a - rand() % range;
	return material;
}

void Game::drawCircle(int xc, int yc, int x, int y, int PixelTypeID)
{
	changeCellType(xc + x, yc + y, PixelTypeID, 0); // BL
	changeCellType(xc - x, yc + y, PixelTypeID, 0); // BR
	//changeCellType(xc + x, yc - y, PixelTypeID, 0);
	//changeCellType(xc - x, yc - y, PixelTypeID, 0);
	//changeCellType(xc + y, yc + x, PixelTypeID, 0);
	//changeCellType(xc - y, yc + x, PixelTypeID, 0);
	//changeCellType(xc + y, yc - x, PixelTypeID, 0);
	//changeCellType(xc - y, yc - x, PixelTypeID, 0);
}

void Game::mouseDraw(int mx, int my, int radius, int chance, int CellTypeID, int CellDrawShape, int range)
{
	const int x = mx / cellScale;
	const int y = my / cellScale;

	if (outOfBounds(x, y)) return;

	if (CellDrawShape == 0) {
		int r2 = radius * radius;
		int area = r2 << 2;
		int rr = radius << 1;

		for (int i = 0; i < area; i++) {
			int tx = (i % rr) - radius;
			int ty = (i / rr) - radius;

			if (tx * tx + ty * ty <= r2 && rand() % (101 - chance) == 0)
				changeCellType(x + tx, y + ty, CellTypeID, range);
		}
	}
	else if (CellDrawShape == 1) {
		int tX = 0;
		int tY = radius;
		int d = 3 - 2 * radius;
		drawCircle(x, y, tX, tY, CellTypeID);
		while (tY >= tX)
		{
			tX++;
			if (d > 0){
				tY--;
				d = d + 4 * (tX - tY) + 10;
			} 
			else d = d + 4 * tX + 6;
			drawCircle(x, y, tX, tY, CellTypeID);
		}
	}
	else if (CellDrawShape == 2) {
		for (int tx = -radius; tx < radius; ++tx)
			if (rand() % (101 - chance) == 0)
				changeCellType(x + tx, y, CellTypeID, range);
	}
	else if (CellDrawShape == 3) {
		for (auto ty = -radius/2; ty < radius/2; ++ty)
			for (auto tx = -radius/2; tx < radius/2; ++tx)
				if (rand() % (101 - chance) == 0)
					changeCellType(x + tx, y + ty, CellTypeID, range);
	}
}

void Game::changeCellType(int x, int y, int CellTypeID, int range)
{
	if (outOfBounds(x, y)) return;
	Cell& c = cells[cellIdx(x, y)];
	c.flag = true;

	if (CellTypeID == 0) c.type = Types[CellTypeID]; //varyPixelColour(5, CellTypeID);
	else c.type = varyPixelColour(range, CellTypeID);
}

void Game::swapCells(Cell& c1, Cell& c2)
{
	CellType t = c1.type;
	c1.type = c2.type;
	c2.type = t;

	c1.flag = true;
	c2.flag = true;
}

bool Game::checkDensity(Cell& c1, int delX, int delY)
{
	if (outOfBounds(c1.x + delX, c1.y + delY)) return false;
	Cell& c2 = cells[cellIdx(c1.x + delX, c1.y + delY)];

	if (c1.type.d <= c2.type.d) return false;
	swapCells(c1, c2);

	return true;
}

// TODO: update velocity increase to deltaTime based.
//		i.e: velocity += accel * deltaTime <-- actually increasing acceleration with time
// Current rand is impacting performance. gpu pseudo rand? 
// can't send each rand to gpu each time, want to group them together..
void Game::updateSand(Cell& c)
{
	//for (int dY = 0; dY < c.v; dY++)
	//	for (int dX = 0; dX < c.v; dX++) {
	//	
	//	}
	
	
	if		(checkDensity(c,  0, 1)) return;
	else if (checkDensity(c,  1, 1)) return;
	else if (checkDensity(c, -1, 1)) return;
	
		//c.v = 1; // didnt move so reset velocity. 
		//++c.v = std::clamp(c.v, 0, c.type.tV); // this looks proper autistic
}

void Game::updateWater(Cell& c)
{
	if		(checkDensity(c,  0, 1));
	else if	(checkDensity(c, -1, 1));
	else if (checkDensity(c,  1, 1));
	else if (checkDensity(c, -1, 0));
	else if (checkDensity(c,  1, 0));
}