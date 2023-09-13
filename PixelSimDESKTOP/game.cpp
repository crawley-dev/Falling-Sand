#pragma once

#include "game.h"

Game::Game() {}
Game::~Game() {}

void Game::init(std::vector<GLubyte> pixelData, int textureW, int textureH)
{
	textureData = pixelData;
	texW = textureW;
	texH = textureH;

//	Types[0] = AIR	 = CellType(0, 188, 231, 255, 255, 0	);
	Types[0] = AIR	 = CellType(0, 255, 255, 255, 255, 0	);
	Types[1] = SAND	 = CellType(1, 245, 215, 176, 255, 1600);
	Types[2] = WATER = CellType(2, 20 , 20 , 255, 100, 997	);
	Types[3] = ROCK  = CellType(3, 200, 200, 200, 255, 2000);

	cells.clear();

	const int initType = 0;
	// init all pixels as air.
	for (int y = 0; y < texH; ++y)
		for (int x = 0; x < texW; ++x) {
			createCell(initType, false, texIdx(x, y), x, y, initType);
			updatePixel(x,y, Types[initType].r, Types[initType].g, Types[initType].b, Types[initType].a);
			cells[cellIdx(x,y)].flag = false;
		}

	printf("Cells length: %d\n", cells.size());
	printf("width: %d\n", textureW);
	printf("height: %d\n", textureH);
	printf("\n");
}

void Game::reload(std::vector<GLubyte> pixelData, int newTexW, int newTexH)
{
	const int deltaW = newTexW - texW;
	const int deltaH = newTexH - texH;
	texW = newTexW;
	texH = newTexH;

	//......
}

void Game::update()
{
	if (texH == 0 || texW == 0) return;

#if 0
	for (int y = texH - 1; y > 0; --y)	
		for (int x = 0; x < texW; x++) {
			Pixel& p = cells[pixIdx(x, y)];
		
			calcNewPosition(p);
			updatePixel(x, y, p.type.r, p.type.g, p.type.b, p.type.a);
			cells[pixIdx(x, y)].flag = false;
		}
#else
	for (Cell& c : cells)
	{
		cellUpdate(c);
		updatePixel(c.x, c.y, c.type.r, c.type.g, c.type.b, c.type.a);
		c.flag = false; // whats the point of set true then straight to false ??
	}
#endif
}

void Game::cellUpdate(Cell& p)
{
	if (p.type.id == 0 || p.type.id == 3) return; // Air | Rock
	else if (p.type.id == 1 || p.type.id == 2) // Sand
	{	
#if false

#elif 2
		if		(checkDensity(p, 0, 1));
		else if (checkDensity(p, -1, 1));
		else if (checkDensity(p, 1, 1));
		else if (p.type.id == 2 && checkDensity(p, -1, 0));
		else if (p.type.id == 2 && checkDensity(p, 1, 0));
#else 
		if (checkDensity(p.x, p.y, p.x, p.y + 1));							// check vertical
		else if (checkDensity(p.x, p.y, p.x - 1, p.y + 1));				// check bot left
		else if (checkDensity(p.x, p.y, p.x + 1, p.y + 1));				// check bot right
		else if (p.type.id == 2 && checkDensity(p.x, p.y, p.x - 1, p.y));	// water check L
		else if (p.type.id == 2 && checkDensity(p.x, p.y, p.x + 1, p.y));	// water check R

#endif	
	}
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

void Game::changeCellType(int x, int y, int CellTypeID)
{
	if (outOfBounds(x, y)) return;
	Cell& p = cells[cellIdx(x, y)];
	if (CellTypeID == 0) cells[cellIdx(x, y)].type = varyPixelColour(5, CellTypeID);
	else cells[cellIdx(x, y)].type = varyPixelColour(20, CellTypeID);
	cells[cellIdx(x, y)].flag = true;
}

void Game::mouseDraw(int x, int y, int radius, int PixelTypeID)
{
	if (outOfBounds(x, y)) return;
	for (auto a = -radius/2; a < radius/2; ++a)
		for (auto b = -radius/2; b < radius/2; ++b)
			changeCellType(x + a, y + b, PixelTypeID);
}

void Game::swapCells(int x1, int y1, int x2, int y2)
{
	if (outOfBounds(x1, y1) || outOfBounds(x2, y2)) return;

	Cell& p1 = cells[cellIdx(x1, y1)];
	Cell& p2 = cells[cellIdx(x2, y2)];

	if (!p1.flag && !p2.flag) {
		int id1 = p1.type.id;
		p1.type = p2.type;
		p2.type = Types[id1];

		p1.flag = true;
		p2.flag = true;
	}
}

void Game::swapCells(Cell& p1, Cell& p2)
{
	if (outOfBounds(p1.x, p1.y) || outOfBounds(p2.x, p2.y) || p1.flag || p2.flag);

	int id1 = p1.type.id;
	p1.type = p2.type;
	p2.type = Types[id1];

	p1.flag = true;
	p2.flag = true;
}

bool Game::checkDensity(int x1, int y1, int x2, int y2) 
{ 
	/*if (cells[pixIdx(x1, y1)].type.d > cells[pixIdx(x2, y2)].type.d &&
		cells[pixIdx(x1, y1)].flag == false && cells[pixIdx(x2,y2)].flag == false) 
	{
		swapcells(x1, y1, x2, y1);
		return true;
	}*/

	if (cells[cellIdx(x1, y1)].type.d < cells[cellIdx(x2, y2)].type.d) return false;
	swapCells(x1, y1, x2, y2);
	return true;
}

bool Game::checkDensity(Cell& p1, int delX, int delY)
{
	Cell& p2 = cells[cellIdx(p1.x + delX, p1.y + delY)];
	if (p1.type.d > p2.type.d) return false;
	swapCells(p1, p2);
	return true;
}

#if false
if (texH == 0) return;
static int count = 0;
const int y = std::max((count % texH), 0);
count++;
printf("%d\n", y);

//for (int y = texH - 1; y > 0; --y)	
for (int x = 0; x < texW; x++) {
	Pixel& p = cells[pixIdx(x, y)];

	if (p.type.g != 255) {
		p.type.r = 255;
		p.type.g = 255;
		p.type.b = 255;
		p.type.a = 255;
	}
	else {
		p.type.r = 255;
		p.type.g = 0;
		p.type.b = 0;
		p.type.a = 255;
	}
	//calcNewPosition(p);
	updatePixel(x, y, p.type.r, p.type.g, p.type.b, p.type.a);
	//cells[pixIdx(x, y)].flag = false;
}
#endif