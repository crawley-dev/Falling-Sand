#pragma once
#include "pch.h"
#include "game.h"

Game::Game() {}
Game::~Game() {}

// annoying that texture --> tex for w,h but not data!!
void Game::init(std::vector<GLubyte>& texData, int textureW, int textureH, int scale)
{
	cellScale   = scale;
	textureData = texData;
	texW        = textureW;
	texH        = textureH;
	cellW       = texW / cellScale;
	cellH       = texH / cellScale;

	Types.clear();
	EMPTY	 = CellType(0, 50 , 50 , 50 , 255, 0   ); Types.push_back(EMPTY   );
	SAND     = CellType(1, 245, 215, 176, 255, 1600); Types.push_back(SAND    );
	WATER	 = CellType(2, 20 , 20 , 255, 125, 997 ); Types.push_back(WATER   );
	CONCRETE = CellType(3, 200, 200, 200, 255, 2000); Types.push_back(CONCRETE);
	ALIVE	 = CellType(4, 0  , 255, 30 , 255, 0   ); Types.push_back(ALIVE	  );

	cells.clear(); // once upon a time, it didn't reset cell arr on reload :/ oops
	cells.reserve(cellW * cellH); // optimises out vector re-location as it grows.

	for (int y = 0; y < cellH; ++y)
		for (int x = 0; x < cellW; ++x) { 
			cells.emplace_back(x, y, EMPTY, false); // doesn't create a new object, then copy it to vec
			updatePixel(cells[cellIdx(x,y)]);
		}
}

// this can be more efficient, currently creates and copies over a vector, could inline it.
void Game::reload(std::vector<GLubyte>& newTexData, int newTexW, int newTexH, int newScaleFactor)
{
	int newCellW = newTexW / newScaleFactor;
	int newCellH = newTexH / newScaleFactor;
	std::vector<Cell> newCells;
	newCells.reserve(newCellW * newCellH);

	for (int y = 0; y < newCellH; ++y)
		for (int x = 0; x < newCellW; ++x)
			if (outOfBounds(x, y))
				newCells.emplace_back(x, y, EMPTY, false);
			else
				newCells.push_back(cells[cellIdx(x,y)]);

	cellScale	= newScaleFactor;
	textureData = newTexData;
	texW        = newTexW;
	texH        = newTexH;
	cellW		= newCellW;
	cellH		= newCellH;
	cells		= newCells;
}

void Game::loadImage(std::vector<GLubyte>& imageData, int imageW, int imageH)
{
	if (imageData.size() > textureData.size()) {
		imageW = texW;
		imageH = texH;
	}

	int idx = 0;
	for (int y = 0; y < imageH / cellScale; y++) 
		for (int x = 0; x < imageW / cellScale; x++) { // how many cells to traverse across. 
 			Cell& c = cells[cellIdx(x, y)]; 
			c.flag   = true;
			c.type   = WATER;
			c.type.r = imageData[idx + 0];
			c.type.g = imageData[idx + 1];
			c.type.b = imageData[idx + 2];
			c.type.a = imageData[idx + 3]; // equivalent to 255 for RGB textures.
			idx     += 4; // don't actually know if this is faster than doing all the multiplication??
			updatePixel(c); // why no wokr >:O .. freezes the grid ? fixed lol
		}
}

void Game::reset(bool& resetSim)
{
	cells.clear();
	cells.reserve(cellW * cellH);

	for (int y = 0; y < cellH; ++y)
		for (int x = 0; x < cellW; ++x) {
			cells.emplace_back(x, y, EMPTY, false);
			updatePixel(cells[cellIdx(x,y)]);
		}
	resetSim = false;
}

void Game::update(interfaceData& data)
{
	if (texH == 0 || texW == 0) return;

	if (!data.runSim) {
		for (Cell& c : cells) 
			updatePixel(c);
		return;
	}

	if (data.playGameOfLife) 
		gameOfLifeUpdate(data);
	else if (data.scanTopDown) {
		for (Cell& c : cells) {
			cellUpdate(c);
			c.flag = false;
			updatePixel(c);
		}
	}
	else { // scan Down->Top
		for (int y = cellH - 1; y >= 0; y--)
			for (int x = cellW - 1; x >= 0; x--) {
				Cell& c = cells[cellIdx(x, y)];
				updatePixel(c); // flipped the order .. and it works !!
				cellUpdate(c);
				c.flag = false;
			}
	}

	if (!data.scanTopDown && data.frame % 3 == 0 || data.scanTopDown) // WUNDERBAR
		data.scanTopDown = altCheck = !data.scanTopDown;
	data.frame++;
}

// POSSIBLE IMPROVEMENTS
// - checkout quadtree approach.
// - Add 1 layer of padding, no need for out of bounds check 
// - store states of all neighbour cells, only need to check 10% as many states
//	 then update a cell's neighbour state when its value is changed. ?? idk man
void Game::gameOfLifeUpdate(interfaceData& data)
{
	std::vector<Cell> nextFrameCells = cells; // Copying all cells to new vec is current bottleneck
	for (int y = 1; y < cellH - 2; y++)		  // doesn't check edge cells, can remove if statement & unroll adjacentcy for loop
		for (int x = 1; x < cellW - 2; x++) {
			Cell& c = nextFrameCells[cellIdx(x, y)];
			//if (c.type.id != EMPTY.id || c.type.id != ALIVE.id) c.type.id = EMPTY.id; // CONSUME SAND, GROW

			//for (int i = -1; i <= 1; i++)
			//	for (int j = -1; j <= 1; j++) {
			//		if (outOfBounds(x + i, y + j) || i == 0 && j == 0) continue; // bottleneck
			//		if (cells[cellIdx(x + i, y + j)].type.id == ALIVE.id)		 // bottleneck
			//			adjAlive++;
			//	}

			int adjAlive = 0;
			adjAlive += (cells[cellIdx(x - 1, y - 1)].type.id == ALIVE.id); // TL
			adjAlive += (cells[cellIdx(x + 0, y - 1)].type.id == ALIVE.id); // TM
			adjAlive += (cells[cellIdx(x + 1, y - 1)].type.id == ALIVE.id); // TR
			adjAlive += (cells[cellIdx(x - 1, y + 0)].type.id == ALIVE.id); // ML
			adjAlive += (cells[cellIdx(x + 1, y + 0)].type.id == ALIVE.id); // MR
			adjAlive += (cells[cellIdx(x - 1, y + 1)].type.id == ALIVE.id); // BL
			adjAlive += (cells[cellIdx(x + 0, y + 1)].type.id == ALIVE.id); // BM
			adjAlive += (cells[cellIdx(x + 1, y + 1)].type.id == ALIVE.id); // BR

			if (c.type.id == ALIVE.id)
				if (adjAlive != 2 && adjAlive != 3) c.type = EMPTY; // seems to work fine 
				else c.type = ALIVE;
			else if (c.type.id == EMPTY.id)
				if (adjAlive == 3)
					c.type = ALIVE;

			updatePixel(c);
		}
	cells = nextFrameCells; // Copying all cells is current bottleneck
}

void Game::cellUpdate(Cell& c)
{
	if (c.flag) return;
	switch (c.type.id) {
		case 1: updateSand(c); return;
		case 2: updateWater(c); return;
	}
}

/*--------------------------------------------------------------------------------------
---- Cell / Pixel Updates --------------------------------------------------------------
--------------------------------------------------------------------------------------*/

#if false
void Game::updatePixel(Cell& c)
{
	const int idx 
	textureData[]
}
#else
void Game::updatePixel(Cell& c) // dunno how to make this faster ... but its slow..
{
	for (int tY = 0; tY < cellScale; tY++)
		for (int tX = 0; tX < cellScale; tX++) {
			int idx = texIdx((c.x * cellScale) + tX, (c.y * cellScale) + tY);
			textureData[idx + 0] = c.type.r;
			textureData[idx + 1] = c.type.g;
			textureData[idx + 2] = c.type.b;
			textureData[idx + 3] = c.type.a;
		}
}
#endif

// this function is quite slow...
CellType Game::varyPixelColour(int range, int cellTypeID)
{
	if (range <= 0 || cellTypeID == EMPTY.id) return Types[cellTypeID];
	CellType material = Types[cellTypeID];
	material.r = std::clamp(material.r - rand() % range, 0, 255);
	material.g = std::clamp(material.g - rand() % range, 0, 255);
	material.b = std::clamp(material.b - rand() % range, 0, 255);
	material.a = std::clamp(material.a - rand() % range, 0, 255);
	return material;
}

void Game::changeCellType(int x, int y, int cellTypeID, int range)
{
	if (outOfBounds(x, y)) return;
	Cell& c = cells[cellIdx(x, y)];
	c.flag = true;
	c.type = varyPixelColour(range, cellTypeID);
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
	if (c1.type.d <= c2.type.d) return false; // density check

	swapCells(c1, c2);
	return true;
}

/*--------------------------------------------------------------------------------------
---- Material Update -------------------------------------------------------------------
--------------------------------------------------------------------------------------*/

// TODO: update velocity increase to deltaTime based.
//		i.e: velocity += accel * deltaTime <-- actually increasing acceleration with time
// rand() function is slow, SplitMix64 is better, but about as optimal 
void Game::updateSand(Cell& c)
{
	if (checkDensity(c, 0, 1)) return;
	checkDensity(c, randOffset(), 1);
}

void Game::updateWater(Cell& c)
{
	if (checkDensity(c,  0, 1)) return;
	if (altCheck) {
		if (checkDensity(c, randOffset(), 0)) return;
		checkDensity(c, randOffset(), 1);
	}
	else {
		if (checkDensity(c, randOffset(), 1)) return;
		checkDensity(c, randOffset(), 0);
	}
}

/*--------------------------------------------------------------------------------------
---- Primitive Algorithms -------------------------------------------------------------- 
--------------------------------------------------------------------------------------*/

void Game::mouseDraw(int mx, int my, int radius, int chance, int CellTypeID, int CellDrawShape, int range)
{
	const int x = mx / cellScale;
	const int y = my / cellScale;

	// edge cases.
	if (outOfBounds(x, y))
		return;
	else if (radius == 1)
		changeCellType(x, y, CellTypeID, range);

	switch (CellDrawShape) { // clean, but lots of repeat parameters.
	case 0: circleFillAlgorithm(x, y, radius, CellTypeID, range, chance); return;
	case 1: circleOutlineAlgorithm(x, y, radius, CellTypeID, range); return;
	case 2:	lineAlgorithm(x, y, radius, CellTypeID, range, chance); return;
	case 3:	squareAlgorithm(x, y, radius, CellTypeID, range, chance); return;
	}
}


void Game::circleFillAlgorithm(int x, int y, int radius, int CellTypeID, int range, int chance)
{
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
void Game::circleOutlineAlgorithm(int x, int y, int radius, int CellTypeID, int range)
{
	int tX = 0;
	int tY = radius;
	int d = 3 - 2 * radius;
	drawCircle(x, y, tX, tY, CellTypeID, range);
	while (tY >= tX) {
		tX++;
		if (d > 0) {
			tY--;
			d = d + 4 * (tX - tY) + 10;
		}
		else d = d + 4 * tX + 6;
		drawCircle(x, y, tX, tY, CellTypeID, range);
	}
}
void Game::drawCircle(int xc, int yc, int x, int y, int PixelTypeID, int range)
{
	changeCellType(xc + x, yc + y, PixelTypeID, range); // BL
	changeCellType(xc - x, yc + y, PixelTypeID, range); // BR
	changeCellType(xc + x, yc - y, PixelTypeID, range);
	changeCellType(xc - x, yc - y, PixelTypeID, range);
	changeCellType(xc + y, yc + x, PixelTypeID, range);
	changeCellType(xc - y, yc + x, PixelTypeID, range);
	changeCellType(xc + y, yc - x, PixelTypeID, range);
	changeCellType(xc - y, yc - x, PixelTypeID, range);
}
void Game::lineAlgorithm(int x, int y, int radius, int CellTypeID, int range, int chance)
{
	for (int tx = -radius; tx < radius; ++tx)
		if (rand() % (101 - chance) == 0)
			changeCellType(x + tx, y, CellTypeID, range);
}
void Game::squareAlgorithm(int x, int y, int radius, int CellTypeID, int range, int chance)
{
	for (auto ty = -radius / 2; ty < radius / 2; ++ty)
		for (auto tx = -radius / 2; tx < radius / 2; ++tx)
			if (rand() % (101 - chance) == 0)
				changeCellType(x + tx, y + ty, CellTypeID, range);
}