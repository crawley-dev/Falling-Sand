#pragma once

#include "game.h"

Game::Game() {}
Game::~Game() {}

// annoying that texture --> tex for w,h but not data!!
void Game::init(std::vector<GLubyte> texData, int textureW, int textureH)
{
	textureData = texData;
	texW = textureW;
	texH = textureH;

//	Types[0] = AIR	 = CellType(0, 188, 231, 255, 255, 0	);
//	Types[0] = AIR	 = CellType(0, 255, 255, 255, 255, 0	);
	Types[0] = AIR	 = CellType(0,  44,  44,  44, 255,	0	);
	Types[1] = SAND	 = CellType(1, 245, 215, 176, 255, 1600	);
	Types[2] = WATER = CellType(2, 20 , 20 , 255, 100, 997	);
	Types[3] = ROCK  = CellType(3, 200, 200, 200, 255, 2000	);
	

	cells.clear(); // once upon a time, it didn't reset cell arr on reload :/ oops

	const int initType = 0;
	for (int y = 0; y < texH; ++y)
		for (int x = 0; x < texW; ++x) {
			createCell(0, false, cellIdx(x, y), x, y, initType);
			updatePixel(x,y, Types[initType].r, Types[initType].g, Types[initType].b, Types[initType].a);
		}
	
	//printf("texureData Size: %d\n", (int)textureData.size());
	//printf("cells Size: %d\n", (int)cells.size());
	//printf("width: %d\n", texW);
	//printf("height: %d\n", texH);
	//printf("\n");
}

void Game::reload(std::vector<GLubyte> pixelData, int newTexW, int newTexH)
{
	const int deltaW = newTexW - texW;
	const int deltaH = newTexH - texH;
	texW = newTexW;
	texH = newTexH;

	//......
}

void Game::reset(int CellTypeID, bool& resetSim)
{
	cells.clear();

	for (int y = 0; y < texH; ++y)
		for (int x = 0; x < texW; ++x) {
			createCell(0, false, cellIdx(x, y), x, y, CellTypeID);
			updatePixel(x, y, Types[CellTypeID].r, Types[CellTypeID].g, Types[CellTypeID].b, Types[CellTypeID].a);
		}
	resetSim = false;
}

void Game::update()
{
	if (texH == 0 || texW == 0) return;

#if 1 // currently flickers..
	for (int y = texH - 1; y > 0; --y)	
		for (int x = 0; x < texW; ++x) {
			Cell& c = cells[cellIdx(x, y)];
		
			cellUpdate(c);
			updatePixel(c.x, c.y, c.type.r, c.type.g, c.type.b, c.type.a);
			c.flag = false;
		}
	//for (Cell& c : cells) c.flag = false;
#elif 1
	for (Cell& c : cells)
	{
		cellUpdate(c);
		updatePixel(c.x, c.y, c.type.r, c.type.g, c.type.b, c.type.a);
		c.flag = false; // whats the point of set true then straight to false ??
	}
#else
	for (auto c = cells.rbegin(); c != cells.rend(); ++c)
	{
		cellUpdate(c);
		updatePixel(c.x, c.y, c.type.r, c.type.g, c.type.b, c.type.a);
		*c.flag = false; // whats the point of set true then straight to false ??
	}
#endif
}

void Game::cellUpdate(Cell& c)
{
#if false
	if (c.type.id == 0 || c.type.id == 3) return;
	else if (c.type.id == 1) updateSand(c);
	else if (c.type.id == 2) updateWater(c);
#elif true
	if (c.type.id == 0 || c.type.id == 3) return; // Air | Rock
	else if (c.type.id == 1 || c.type.id == 2) // Sand
	{
		if		(checkDensity(c,  0, 1));
		else if (checkDensity(c, -1, 1));
		else if (checkDensity(c,  1, 1));
		else if (c.type.id == 2 && checkDensity(c, -1, 0));
		else if (c.type.id == 2 && checkDensity(c, 1, 0));
	}
#elif false
	if (c.type.id == 0 || c.type.id == 3) return; // Air | Rock
	else if (c.type.id == 1 || c.type.id == 2) // Sand
	{
		if (checkDensity(c.x, c.y, c.x, c.y + 1));							// check vertical
		else if (checkDensity(c.x, c.y, c.x - 1, c.y + 1));				// check bot left
		else if (checkDensity(c.x, c.y, c.x + 1, c.y + 1));				// check bot right
		else if (c.type.id == 2 && checkDensity(c.x, c.y, c.x - 1, c.y));	// water check L
		else if (c.type.id == 2 && checkDensity(c.x, c.y, c.x + 1, c.y));	// water check R

	}
#endif	
}

void Game::createCell(int range, bool flag, int texIdx, int x, int y, int PixelTypeID)
{
	if (outOfBounds(x, y)) return;
	cells.push_back(Cell(flag, texIdx, x, y, varyPixelColour(range, PixelTypeID)));
}

void Game::updatePixel(int x, int y, int r, int g, int b, int a)
{
	if (outOfBounds(x, y)) return;

	int idx = texIdx(x, y);
	textureData[idx + 0] = r;
	textureData[idx + 1] = g;
	textureData[idx + 2] = b;
	textureData[idx + 3] = a;
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


void Game::mouseDraw(int x, int y, int radius, int CellTypeID, int range)
{
#if true
	const int chance = 10; // 1 out of 100
	int r2 = radius * radius;
	int area = r2 << 2;
	int rr = radius << 1;

	for (int i = 0; i < area; i++)
	{
		int tx = (i % rr) - radius;
		int ty = (i / rr) - radius;

		if (tx * tx + ty * ty <= r2 && rand() % chance == 0)
			changeCellType(x + tx, y + ty, CellTypeID, range);
	}
#else
	if (outOfBounds(x, y) || outOfBounds(x - radius, y - radius) || outOfBounds(x + radius, y + radius)) return;
	printf("coords: (%d,%d)\n", x, y);

	for (auto ty = 0; ty < radius; ++ty)
		for (auto tx = 0; tx < radius; ++tx)
			changeCellType(x + tx, y + ty, CellTypeID, range);
#endif
}

void Game::changeCellType(int x, int y, int CellTypeID, int range)
{
	if (outOfBounds(x, y)) return;

	Cell& c = cells[cellIdx(x, y)];
	c.flag = true;

	if (CellTypeID == 0) c.type = Types[CellTypeID];//varyPixelColour(5, CellTypeID);
	else c.type = varyPixelColour(range, CellTypeID);
}

void Game::swapCells(int x1, int y1, int x2, int y2)
{
	if (outOfBounds(x1, y1) || outOfBounds(x2, y2)) return;

	Cell& c1 = cells[cellIdx(x1, y1)];
	Cell& c2 = cells[cellIdx(x2, y2)];

	if (c1.flag || c2.flag) return; 
	CellType t = c1.type;
	c1.type = c2.type;
	c2.type = t;

	c1.flag = true;
	c2.flag = true;
}

void Game::swapCells(Cell& c1, Cell& c2)
{
	if (outOfBounds(c1.x, c1.y) || outOfBounds(c2.x, c2.y) || c1.flag || c2.flag);

	CellType t = c1.type;
	c1.type = c2.type;
	c2.type = t;

	c1.flag = true;
	c2.flag = true;
}

bool Game::checkDensity(int x1, int y1, int x2, int y2) 
{ 
	if (cells[cellIdx(x1, y1)].type.d < cells[cellIdx(x2, y2)].type.d) return false;
	swapCells(x1, y1, x2, y2);
	return true;
}

bool Game::checkDensity(Cell& c1, int delX, int delY)
{
	/*
	if (cellIdx(c1.x + delX, c1.y + delY) >= texW * texH) {
		printf("x: %d\n", c1.x + delX);
		printf("y: %d\n", c1.y + delY);
		printf("texW: %d\n", texW);
		printf("texH: %d\n", texH);
	}*/

	if (outOfBounds(c1.x + delX, c1.y + delY)) return false;
	Cell& c2 = cells[cellIdx(c1.x + delX, c1.y + delY)];
	if (c1.type.d <= c2.type.d) return false;
	swapCells(c1, c2);
	return true;
}

void Game::updateSand(Cell& c)
{
	if		(checkDensity(c,  0, 1));
	else if (checkDensity(c, -1, 1));
	else if (checkDensity(c,  1, 1));
}

void Game::updateWater(Cell& c)
{
	if		(checkDensity(c,  0, 1)					 );
	else if (checkDensity(c, -1, 1)					 );
	else if (checkDensity(c,  1, 1)					 );
	else if (c.type.id == 2 && checkDensity(c, -1, 0));
	else if (c.type.id == 2 && checkDensity(c, 1, 0) );
}

#if false
if (texH == 0) return;
static int count = 0;
const int y = std::max((count % texH), 0);
count++;
printf("%d\n", y);

//for (int y = texH - 1; y > 0; --y)	
for (int x = 0; x < texW; x++) {
	Pixel& c = cells[pixIdx(x, y)];

	if (c.type.g != 255) {
		c.type.r = 255;
		c.type.g = 255;
		c.type.b = 255;
		c.type.a = 255;
	}
	else {
		c.type.r = 255;
		c.type.g = 0;
		c.type.b = 0;
		c.type.a = 255;
	}
	//calcNewPosition(c);
	updatePixel(x, y, c.type.r, c.type.g, c.type.b, c.type.a);
	//cells[pixIdx(x, y)].flag = false;
}
#endif