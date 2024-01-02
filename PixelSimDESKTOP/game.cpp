#pragma once
#include "pch.h"
#include "game.h"

Game::Game() {}
Game::~Game() {}

void Game::init(u16 textureWidth, u16 textureHeight, u8 newScaleFactor)
{
	scaleFactor = newScaleFactor;
	cellWidth = textureWidth / scaleFactor;
	cellHeight = textureHeight / scaleFactor;

	materials.clear();
	materials.resize(MaterialID::COUNT); // just so i can [] access, clear code.
	materials[MaterialID::EMPTY]	 = Material(50 , 50 , 50 , 255, 0   );
	materials[MaterialID::SAND]		 = Material(245, 215, 176, 255, 1600);
	materials[MaterialID::WATER]	 = Material(20 , 20 , 255, 125, 997 );
	materials[MaterialID::CONCRETE]	 = Material(200, 200, 200, 255, 2000);
	materials[MaterialID::GOL_ALIVE] = Material(0  , 255, 30 , 255, 0   );

	// generate 'nVariant' number of colour variations per material. for spice..
	//for (Material& mat : materials) {
	//	mat.variants.clear();
	//	mat.variants.reserve(nVariants);
	//	for (u8 i = 0; i < nVariants; ++i)
	//		mat.variants.push_back({
	//			getRand<u8>(0, nVariants),
	//			getRand<u8>(0, nVariants),
	//			getRand<u8>(0, nVariants)});
	//}

	cells.clear();
	cells.reserve(cellWidth * cellHeight);
	for (u32 i = 0; i < cellWidth * cellHeight; ++i)
		cells.emplace_back(MaterialID::EMPTY, false, getRand<u8>(0, nVariants), 0);
}

void Game::reload(u16 textureWidth, u16 textureHeight, u8 newScaleFactor)
{
	u16 newCellWidth = textureWidth / newScaleFactor;
	u16 newCellHeight = textureHeight / newScaleFactor;
	std::vector<Cell> newCells;
	for (u16 y = 0; y < newCellHeight; ++y)
		for (u16 x = 0; x < newCellWidth; ++x)
			if (outOfBounds(x, y))
				newCells.emplace_back(MaterialID::EMPTY, false, getRand<u8>(0, nVariants), 0);
			else
				newCells.push_back(cells[cellIdx(x,y)]);

	cells = newCells;
	cellWidth = newCellWidth;
	cellHeight = newCellHeight;
	scaleFactor = newScaleFactor;
}

void Game::reset()
{
	cells.clear();
	cells.reserve(cellWidth * cellHeight);
	for (u32 i = 0; i < cellWidth * cellHeight; ++i)
		cells.emplace_back(MaterialID::EMPTY, false, getRand<u8>(0, nVariants), 0);
}

/*--------------------------------------------------------------------------------------
---- Simulation Update Routines --------------------------------------------------------
--------------------------------------------------------------------------------------*/

void Game::updateSim(interfaceData& iData)
{
	if (!iData.runSim) return;
	
	switch (iData.updateMode) {
	case Update::TOP_DOWN:		topDown_Update();	break;
	case Update::BOTTOM_UP:		bottomUp_Update();	break;
	case Update::SNAKE:			snake_Update();		break;
	case Update::GAME_OF_LIFE:	gol_Update();		break;
	}

	if (iData.updateMode == Update::TOP_DOWN)
		iData.updateMode = Update::BOTTOM_UP;
	else if (iData.updateMode == Update::BOTTOM_UP && iData.frame % 3 == 0)
		iData.updateMode = Update::TOP_DOWN;
	iData.frame++;
}

void Game::topDown_Update()
{
	for (u16 y = 0; y < cellHeight; ++y)
		for (u16 x = 0; x < cellWidth; ++x)
			updateCell(x, y);
}

void Game::bottomUp_Update()
{
	for (u16 y = cellHeight - 1; y >= 0; --y)
		for (u16 x = cellWidth - 1; x >= 0; --x)
			updateCell(x, y);
}

// >>>>>>>>>>^
// ^<<<<<<<<<<
// >>>>>>>>>>^
void Game::snake_Update()
{
	for (u16 y = cellHeight - 1; y >= 0; --y)
		if ((cellHeight - y) % 2 == 0) // --> 
			for (u16 x = 0; x < cellWidth; ++x)
				updateCell(x, y);
		else					 // <--
			for (u16 x = cellWidth - 1; x >= 0; --x)
				updateCell(x, y);
}

void Game::gol_Update()
{
	std::vector<Cell> nextFrameCells = cells; // Copying all cells to new vec is current bottleneck
	for (u16 y = 1; y < cellHeight - 2; y++)  // doesn't check edge cells, can remove if statement & unroll adjacentcy for loop
		for (u16 x = 1; x < cellWidth - 2; x++) {
			Cell& c = nextFrameCells[cellIdx(x, y)];
			if (c.matID != MaterialID::GOL_ALIVE || c.matID != MaterialID::EMPTY) continue;

			u8 adjAlive = 0;
			adjAlive += (cells[cellIdx(x - 1, y - 1)].matID == MaterialID::GOL_ALIVE); // TL
			adjAlive += (cells[cellIdx(x + 0, y - 1)].matID == MaterialID::GOL_ALIVE); // TM
			adjAlive += (cells[cellIdx(x + 1, y - 1)].matID == MaterialID::GOL_ALIVE); // TR
			adjAlive += (cells[cellIdx(x - 1, y + 0)].matID == MaterialID::GOL_ALIVE); // ML
			adjAlive += (cells[cellIdx(x + 1, y + 0)].matID == MaterialID::GOL_ALIVE); // MR
			adjAlive += (cells[cellIdx(x - 1, y + 1)].matID == MaterialID::GOL_ALIVE); // BL
			adjAlive += (cells[cellIdx(x + 0, y + 1)].matID == MaterialID::GOL_ALIVE); // BM
			adjAlive += (cells[cellIdx(x + 1, y + 1)].matID == MaterialID::GOL_ALIVE); // BR

			if (c.matID == MaterialID::GOL_ALIVE)
				if (adjAlive != 2 && adjAlive != 3) c.matID = MaterialID::EMPTY; // seems to work fine 
				else c.matID = MaterialID::GOL_ALIVE;
			else if (c.matID == MaterialID::EMPTY)
				if (adjAlive == 3)
					c.matID = MaterialID::GOL_ALIVE;
		}
	cells = nextFrameCells; // Copying all cells is current bottleneck
}

/*--------------------------------------------------------------------------------------
---- Updating Cells --------------------------------------------------------------------
--------------------------------------------------------------------------------------*/

void Game::updateCell(u16 x, u16 y)
{
	Cell& c = cells[cellIdx(x, y)];
	if (c.updated) return;

	switch (c.matID) {
	case MaterialID::SAND:	updateSand(c, x, y);
	case MaterialID::WATER: updateWater(c, x, y);
	}
}

void Game::updateSand(Cell& c, u16 x, u16 y)
{
	if (trySwap(c, x, y + 1)) return;
	else trySwap(c, x + getRand<u8>(), y + 1);
}

void Game::updateWater(Cell& c, u16 x, u16 y)
{
	if (trySwap(c, x, y + 1)) return;
	else if (trySwap(c, x + getRand<u8>(), y + 1)) return;
	else (trySwap(c, x + getRand<u8>(), y));
}

bool Game::trySwap(Cell& c1, u16 x2, u16 y2) 
{
	if (outOfBounds(x2, y2)) return false;

	Cell& neighbour = cells[cellIdx(x2, y2)]; // density check
	if (materials[c1.matID].d <= materials[neighbour.matID].d) return false;
	
	swapCells(c1, neighbour);
	return true;
}

inline void Game::changeMaterial(u16 x, u16 y, u8 newMaterial)
{
	Cell& c = cells[cellIdx(x, y)];
	c.matID = newMaterial;
	c.updated = true;
}

void Game::swapCells(Cell& c1, Cell& c2)
{
	u8 temp_MaterialID = c1.matID;
	c1.matID = c2.matID;
	c2.matID = temp_MaterialID;
	
	c1.updated = true;
	c2.updated = true;
}

/*--------------------------------------------------------------------------------------
---- Mouse Functions -------------------------------------------------------------------
--------------------------------------------------------------------------------------*/

void Game::mouseDraw(u16 mx, u16 my, u16 size, u8 drawChance, u8 material, u8 shape)
{
	const u32 x = mx / scaleFactor;
	const u32 y = my / scaleFactor;

	// edge cases.
	if (outOfBounds(x, y)) return;
	else if (size == 1)
		changeMaterial(x, y, material);

	switch (shape) { // clean, but lots of repeat parameters.
	case Shape::CIRCLE:			draw_Circle(x, y, size, material, drawChance);	break;
	case Shape::CIRCLE_OUTLINE: draw_CircleOutline(x, y, size, material, drawChance);  break;
	case Shape::LINE:			draw_Line(x, y, size, material, drawChance);	break;
	case Shape::SQUARE:			draw_Square(x, y, size, material, drawChance);	break;
	}
}

void Game::draw_Circle(u16 x, u16 y, u16 size, u8 material, u8 drawChance)
{
	u32 r2 = size * size;
	u32 area = r2 << 2;
	u32 rr = size << 1;

	for (u32 i = 0; i < area; i++) {
		u32 tx = (i % rr) - size;
		u32 ty = (i / rr) - size;

		if (tx * tx + ty * ty <= r2)
			if (rand() % (101 - drawChance) == 0)
				changeMaterial(x + tx, y + ty, material);
	}
}

void Game::draw_CircleSegments(u16 xc, u16 yc, u16 x, u16 y, u8 material)
{
	changeMaterial(xc + x, yc + y, material);
	changeMaterial(xc - x, yc + y, material);
	changeMaterial(xc + x, yc - y, material);
	changeMaterial(xc - x, yc - y, material);
	changeMaterial(xc + y, yc + x, material);
	changeMaterial(xc - y, yc + x, material);
	changeMaterial(xc + y, yc - x, material);
	changeMaterial(xc - y, yc - x, material);
}

void Game::draw_CircleOutline(u16 x, u16 y, u16 size, u8 material, u8 drawChance)
{
	u32 tX = 0;
	u32 tY = size;
	u32 d = 3 - 2 * size;
	draw_CircleSegments(x, y, tX, tY, material);
	while (tY >= tX) {
		++tX;
		if (d > 0) {
			tY--;
			d = d + 4 * (tX - tY) + 10;
		}
		else 
			d = d + 4 * tX + 6;
		draw_CircleSegments(x, y, tX, tY, material);
	}
}

void Game::draw_Line(u16 x, u16 y, u16 size, u8 material, u8 drawChance)
{
	for (s32 tx = -size; tx < size; ++tx)
		if (rand() % (101 - drawChance) == 0)
			changeMaterial(x + tx, y, material);
}

void Game::draw_Square(u16 x, u16 y, u16 size, u8 material, u8 drawChance)
{
	for (s32 ty = -size / 2; ty < size / 2; ++ty)
		for (s32 tx = -size / 2; tx < size / 2; ++tx)
			if (rand() % (101 - drawChance) == 0)
				changeMaterial(x + tx, y + ty, material);
}

/*--------------------------------------------------------------------------------------
---- Updating Texture -------------------------------------------------------------------
--------------------------------------------------------------------------------------*/

std::vector<GLubyte> Game::updateTextureData()
{
	//if (textureData.size() != cells.size() * scaleFactor * scaleFactor * 4) {
	//	printf("Textures of different size!, Difference: %d\n", textureData.size() - (cells.size() * scaleFactor * scaleFactor * 4));
	//	return;
	//}

	std::vector<GLubyte> textureData(cells.size() * scaleFactor * scaleFactor * 4, 255);
	for (int y = 0; y < cellHeight; ++y) {
		for (int x = 0; x < cellWidth; ++x) { 
			Cell& c = cells[cellIdx(x, y)];
			//if (!c.updated) continue;

			Material& material = materials[c.matID];
			const GLubyte red   = material.r;
			const GLubyte green = material.g;
			const GLubyte blue  = material.b;
			const GLubyte alpha = material.a;

			for (int tY = 0; tY < scaleFactor; ++tY)
				for (int tX = 0; tX < scaleFactor; ++tX) {
					u32 idx = textureIdx((x * scaleFactor) + tX, (y * scaleFactor) + tY);
					textureData[idx + 0] = red;
					textureData[idx + 1] = green;
					textureData[idx + 2] = blue;
					textureData[idx + 3] = alpha;
				}
		}
	}
	return textureData;
}

/*
void Game::updateTextureData(std::vector<GLubyte>& textureData)
{
	// current code doesn't account for scale factor..
	//   either:
	//	- row by row and store data, cache is happy?? << WANT
	//	- grid loop (legacy) for each cell.
	
	if (textureData.size() == cells.size() * scaleFactor * scaleFactor * 4) {
		u64 texIdx = 0;
		for (u64 cellIdx = 0; cellIdx < cells.size(); ++cellIdx) {
			Cell& c = cells[cellIdx];
			if (!c.updated) continue; // hasn't moved, so don't change it?
			textureData[texIdx + 0] = materials[c.matID].r; // no variants for now.
			textureData[texIdx + 0] = materials[c.matID].g; // no variants for now.
			textureData[texIdx + 0] = materials[c.matID].b; // no variants for now.
			textureData[texIdx + 0] = materials[c.matID].a; // no variants for now.
			texIdx += 4;
		}
	}
	else { // texture has changed size.
		textureData.clear();
		textureData.reserve(cells.size() * scaleFactor * scaleFactor * 4);
		for (Cell& c : cells) {
			textureData.push_back(materials[c.matID].r);
			textureData.push_back(materials[c.matID].g);
			textureData.push_back(materials[c.matID].b);
			textureData.push_back(materials[c.matID].a);
		}
	}
}
*/

