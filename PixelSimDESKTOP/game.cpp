#pragma once
#include "pch.h"
#include "game.h"

Game::Game() {}
Game::~Game() {}

void Game::init(u16 newTextureWidth, u16 newTextureHeight, u8 newScaleFactor)
{
	scaleFactor = newScaleFactor;
	textureWidth = newTextureWidth;
	textureHeight = newTextureHeight;
	cellWidth = newTextureWidth / scaleFactor;
	cellHeight = newTextureHeight / scaleFactor;

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
	for (s32 i = 0; i < cellWidth * cellHeight; i++) // init cell.updated = true so updateTextureData runs on first time.
		cells.emplace_back(MaterialID::EMPTY, true, getRand<u8>(0, nVariants), 0);
}

void Game::reload(u16 newTextureWidth, u16 newTextureHeight, u8 newScaleFactor)
{
	const u32 newCellWidth = newTextureWidth / newScaleFactor;
	const u32 newCellHeight = newTextureHeight / newScaleFactor;

	std::vector<Cell> newCells;
	newCells.reserve(newCellWidth * newCellHeight);
	for (s32 y = 0; y < newCellHeight; y++)
		for (s32 x = 0; x < newCellWidth; x++)
			if (outOfBounds(x, y))
				newCells.emplace_back(MaterialID::EMPTY, false, getRand<u8>(0, nVariants), 0);
			else
				newCells.push_back(cells[cellIdx(x,y)]);

	cells = newCells;
	cellWidth = newCellWidth;
	cellHeight = newCellHeight;
	textureWidth = newTextureWidth;
	textureHeight = newTextureHeight;
	scaleFactor = newScaleFactor;
}

void Game::reset()
{
	cells.clear();
	cells.reserve(cellWidth * cellHeight);
	for (s32 i = 0; i < cellWidth * cellHeight; i++)
		cells.emplace_back(MaterialID::EMPTY, false, getRand<u8>(0, nVariants), 0);
}

/*--------------------------------------------------------------------------------------
---- Simulation Update Routines --------------------------------------------------------
--------------------------------------------------------------------------------------*/

void Game::updateSim(interfaceData& iData)
{	// keep some form of global index that is incremented with each cell
	// thereby eliminating the need to pass x,y to cellUpdate etc. instead just cell.

	if (!iData.runSim) return;
	
	switch (iData.updateMode) {
	case Update::TOP_DOWN:		topDown_Update();  break;
	case Update::BOTTOM_UP:		bottomUp_Update(); break;
	case Update::SNAKE:			snake_Update();	   break;
	case Update::GAME_OF_LIFE:	gol_Update();	   break;
	}

	//if (iData.updateMode == Update::TOP_DOWN)
	//	iData.updateMode = Update::BOTTOM_UP;
	//else if (iData.updateMode == Update::BOTTOM_UP && iData.frame % 3 == 0)
	//	iData.updateMode = Update::TOP_DOWN;
	//iData.frame++;
}

void Game::topDown_Update()
{
	for (s32 y = 0; y < cellHeight; y++)
		for (s32 x = 0; x < cellWidth; x++) {
			Cell& c = cells[cellIdx(x, y)];
			updateCell(c, x, y);
			c.updated = false;
		}

}

void Game::bottomUp_Update()
{
	for (s32 y = cellHeight - 1; y >= 0; y--)
		for (s32 x = cellWidth - 1; x >= 0; x--) {
			Cell& c = cells[cellIdx(x, y)];
			updateCell(c, x, y);
			c.updated = false;
		}
}

// >>>>>>>>>>^
// ^<<<<<<<<<<
// >>>>>>>>>>^
void Game::snake_Update()
{
	for (s32 y = cellHeight - 1; y >= 0; y--)
		if ((cellHeight - y) % 2 == 0) // --> 
			for (s32 x = 0; x < cellWidth; x++) {
				Cell& c = cells[cellIdx(x, y)];
				updateCell(c, x, y);
				c.updated = false;
			}
		else					 // <--
			for (s32 x = cellWidth - 1; x >= 0; x--) {
				Cell& c = cells[cellIdx(x, y)];
				updateCell(c, x, y);
				c.updated = false;
			}
}

void Game::gol_Update()
{
	std::vector<Cell> nextFrameCells = cells; // Copying all cells to new vec is current bottleneck
	for (s32 y = 1; y < cellHeight - 2; y++)  // doesn't check edge cells, can remove if statement & unroll adjacentcy for loop
		for (s32 x = 1; x < cellWidth - 2; x++) {
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

void Game::updateCell(Cell& c, u16 x, u16 y)
{
	if (c.updated) return;

	switch (c.matID) {
	case MaterialID::SAND:	updateSand(c, x, y);
	case MaterialID::WATER: updateWater(c, x, y);
	}
}

void Game::updateSand(Cell& c, u16 x, u16 y)
{
	if (trySwap(c, x, y + 1)) return;
	else trySwap(c, x + getRand<s8>(), y + 1);
}

void Game::updateWater(Cell& c, u16 x, u16 y)
{
	if (trySwap(c, x, y + 1)) return;
	else if (trySwap(c, x + getRand<s8>(), y + 1)) return;
	else (trySwap(c, x + getRand<s8>(), y));
}

bool Game::trySwap(Cell& c1, u16 x2, u16 y2) 
{
	if (outOfBounds(x2, y2)) return false;

	Cell& neighbour = cells[cellIdx(x2, y2)]; // density check
	if (materials[c1.matID].d <= materials[neighbour.matID].d) return false;
	
	swapCells(c1, neighbour);
	return true;
}

void Game::changeMaterial(u16 x, u16 y, u8 newMaterial)
{
	if (outOfBounds(x, y)) return;
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

	switch (shape) {
	case Shape::CIRCLE:			draw_Circle(x, y, size, material, drawChance);	break;
	case Shape::CIRCLE_OUTLINE: draw_CircleOutline(x, y, size, material, drawChance);  break;
	case Shape::LINE:			draw_Line(x, y, size, material, drawChance);	break;
	case Shape::SQUARE:			draw_Square(x, y, size, material, drawChance);	break;
	}
}

void Game::draw_Circle(u16 x, u16 y, u16 size, u8 material, u8 drawChance)
{
	int r2 = size * size;
	int area = r2 << 2;
	int rr = size << 1;

	for (s32 i = 0; i < area; i++) {
		int tX = (i % rr) - size;
		int tY = (i / rr) - size;

		if (tX * tX + tY * tY <= r2)
			if (getRand<u64>(drawChance, 100) == drawChance)
				changeMaterial(x + tX, y + tY, material);
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
	s32 tX = 0;
	s32 tY = size;
	s32 d = 3 - 2 * size;
	draw_CircleSegments(x, y, tX, tY, material);
	while (tY >= tX) {
		tX++;
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
	for (s32 tX = -size; tX < size; tX++)
		if (getRand<u64>(drawChance, 100) == drawChance)
			changeMaterial(x + tX, y, material);
}

void Game::draw_Square(u16 x, u16 y, u16 size, u8 material, u8 drawChance)
{
	for (s32 tY = -size / 2; tY < size / 2; tY++)
		for (s32 tX = -size / 2; tX < size / 2; tX++)
			if (getRand<u64>(drawChance, 100) == drawChance)
				changeMaterial(x + tX, y + tY, material);
}

/*--------------------------------------------------------------------------------------
---- Updating Texture -------------------------------------------------------------------
--------------------------------------------------------------------------------------*/

void Game::updateTextureData(std::vector<GLubyte>& textureData, u32 width)
{
	if (textureData.size() != cells.size() * scaleFactor * scaleFactor * 4) {
		printf("Textures of different size!, Difference: %d\n", textureData.size() - (cells.size() * scaleFactor * scaleFactor * 4));
		return;
	}

	textureWidth = width;

	for (s32 y = 0; y < cellHeight; y++) {
		for (s32 x = 0; x < cellWidth; x++) { 
			Cell& c = cells[cellIdx(x, y)];
			//if (!c.updated) continue;

			Material& material = materials[c.matID];
			const GLubyte red   = material.r;
			const GLubyte green = material.g;
			const GLubyte blue  = material.b;
			const GLubyte alpha = material.a;

			for (s32 tY = 0; tY < scaleFactor; tY++)
				for (s32 tX = 0; tX < scaleFactor; tX++) {
					//s32 idx = 4 * ((((y * scaleFactor) + tY) * width) + (x * scaleFactor) + tX);
					s32 idx = textureIdx((x * scaleFactor) + tX, (y * scaleFactor) + tY);
					textureData[idx + 0] = red;
					textureData[idx + 1] = green;
					textureData[idx + 2] = blue;
					textureData[idx + 3] = alpha;
				}
		}
	}
}