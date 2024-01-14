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
	nVariants = 20;
	constexpr u8 variation = 15;
	for (Material& mat : materials) {
		mat.variants.clear();
		mat.variants.reserve(nVariants);
		for (u8 i = 0; i < nVariants; ++i)
			mat.variants.push_back({
				getRand<u8>(0, variation),	 // RED
				getRand<u8>(0, variation),	 // GREEN
				getRand<u8>(0, variation),	 // BLUE
				getRand<u8>(0, variation)}); // ALPHA
	}

	cells.clear();
	cells.reserve(cellWidth * cellHeight);
	for (s32 i = 0; i < cellWidth * cellHeight; i++) // init cell.updated = true so updateTextureData runs on first time.
		cells.emplace_back(MaterialID::EMPTY, true, getRand<u8>(0, nVariants), 0);
	sizeChanged = true;
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

	sizeChanged = true;

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
	sizeChanged = true;
}

/*--------------------------------------------------------------------------------------
---- Simulation Update Routines --------------------------------------------------------
--------------------------------------------------------------------------------------*/

void Game::updateSim(interfaceData& data)
{	// keep some form of global index that is incremented with each cell
	// thereby eliminating the need to pass x,y to cellUpdate etc. instead just cell.

	if (!data.runSim) return;
	
	switch (data.scanMode) {
	case Scan::TOP_DOWN:		topDown_Update();  break;
	case Scan::BOTTOM_UP:		bottomUp_Update(); break;
	case Scan::SNAKE:			snake_Update();	   break;
	case Scan::GAME_OF_LIFE:	gol_Update();	   break;
	}

	if (data.updateMode == Update::FLICKER) {
		if (data.scanMode == Scan::TOP_DOWN)
			data.scanMode = Scan::BOTTOM_UP;
		else if (data.scanMode == Scan::BOTTOM_UP && data.frame % 3 == 0)
			data.scanMode = Scan::TOP_DOWN;
	}

	data.frame++;
}

void Game::topDown_Update()
{
	for (s32 y = 0; y < cellHeight; y++)
		for (s32 x = 0; x < cellWidth; x++) {
			Cell& c = cells[cellIdx(x, y)];
			updateCell(c, x, y);
			if (c.updated) toUpdate.push_back(std::pair<u16, u16>(x, y)); // get rid of if statement.
			c.updated = false;
		}
}

void Game::bottomUp_Update()
{
	for (s32 y = cellHeight - 1; y >= 0; y--)
		for (s32 x = cellWidth - 1; x >= 0; x--) {
			Cell& c = cells[cellIdx(x, y)];
			updateCell(c, x, y);
			if (c.updated) toUpdate.push_back(std::pair<u16, u16>(x, y));
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
				if (c.updated) toUpdate.push_back(std::pair<u16, u16>(x, y)); // get rid of if statement.
				c.updated = false;
			}
		else					 // <--
			for (s32 x = cellWidth - 1; x >= 0; x--) {
				Cell& c = cells[cellIdx(x, y)];
				updateCell(c, x, y);
				if (c.updated) toUpdate.push_back(std::pair<u16, u16>(x, y)); // get rid of if statement.
				c.updated = false;
			}
}

void Game::gol_Update()
{
	std::vector<Cell> nextFrameCells = cells; // Copying all cells to new vec is current bottleneck
	for (s32 y = 1; y < cellHeight - 2; y++)  // doesn't check edge cells, can remove if statement & unroll adjacentcy for loop
		for (s32 x = 1; x < cellWidth - 2; x++) {
			Cell& c = nextFrameCells[cellIdx(x, y)];
			//if (c.matID != MaterialID::GOL_ALIVE || c.matID != MaterialID::EMPTY) continue;

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
	case MaterialID::EMPTY:		return;
	case MaterialID::CONCRETE:  return;
	case MaterialID::GOL_ALIVE: return;
	case MaterialID::SAND:	updateSand(c, x, y);  break;
	case MaterialID::WATER: updateWater(c, x, y); break;
	}
}

#define CELL_DISPERSION 1
#if RAND_CELL_UPDATE

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
#else 

void Game::updateSand(Cell& c, u16 x, u16 y)
{
	if (trySwap(c, x, y + 1)) return;
	else if (trySwap(c, x - 1, y + 1)) return;
	else trySwap(c, x + 1, y + 1); 
}

constexpr u8 liquidDispersionRate = 8;
void Game::updateWater(Cell& c, u16 x, u16 y)
{

	if (trySwap(c, x, y + 1)) return;
	else if (trySwap(c, x - 1, y + 1)) return;
	else if (trySwap(c, x + 1, y + 1)) return;

	s8 dispersion = 0;
	for (u8 dX = 0; dX < liquidDispersionRate; dX++) {

			if (outOfBounds(x + dX, y)) break;
			else dispersion = dX;

			if (outOfBounds(x - dX, y)) break;
			else dispersion = -dX;
			if (getRand<u8>() % 2 == 0) dispersion *= -1;
	}

	trySwap(c, x + dispersion, y);
}

#endif

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

	toUpdate.push_back(std::pair<u16,u16>(x,y));
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
---- Updating Texture ------------------------------------------------------------------
--------------------------------------------------------------------------------------*/

void Game::updateTextureData(std::vector<GLubyte>& textureData)
{
	// all this would do is prevent // allow >> if (!c.updated) continue <<
	// re-writing code would be more efficient as it would not include an if statement.
	// can't use a compile-time expression to determine, i.e #ifdef TEST_123, no overhead.

	if (sizeChanged) {
		updateEntireTextureData(textureData);
		sizeChanged = false;
		return;
	}

	for (auto [x,y] : toUpdate) { // cheeky structured binding.
		const Cell& c = cells[cellIdx(x,y)];
		const Material& material = materials[c.matID];

		const GLubyte red = material.r;
		const GLubyte green = material.g;
		const GLubyte blue = material.b;
		const GLubyte alpha = material.a;

		for (s32 tY = 0; tY < scaleFactor; tY++)
			for (s32 tX = 0; tX < scaleFactor; tX++) {
				s32 texIdx = textureIdx((x * scaleFactor) + tX, (y * scaleFactor) + tY);
				textureData[texIdx + 0] = red;
				textureData[texIdx + 1] = green;
				textureData[texIdx + 2] = blue;
				textureData[texIdx + 3] = alpha;
			}
	}

	toUpdate.clear();
}

void Game::updateEntireTextureData(std::vector<GLubyte>& textureData)
{
	for (s32 y = 0; y < cellHeight; y++) {
		for (s32 x = 0; x < cellWidth; x++) {
			const Cell& c = cells[cellIdx(x, y)];
			const Material& material = materials[c.matID];

			const GLubyte red = material.r;
			const GLubyte green = material.g;
			const GLubyte blue = material.b;
			const GLubyte alpha = material.a;

			for (s32 tY = 0; tY < scaleFactor; tY++)
				for (s32 tX = 0; tX < scaleFactor; tX++) {
					s32 idx = textureIdx((x * scaleFactor) + tX, (y * scaleFactor) + tY);
					textureData[idx + 0] = red;
					textureData[idx + 1] = green;
					textureData[idx + 2] = blue;
					textureData[idx + 3] = alpha;
				}
		}
	}
}

void Game::loadImage(std::vector<GLubyte> imageTextureData, u16 imageWidth, u16 imageHeight)
{

}

// Instead of Variant schenanigans, do the 'next' step for image loading, 
// find the material it is closest to, and set it to that!
// So I should probably add more materials.
/*
void Game::loadImage(std::vector<GLubyte> imageTextureData, u16 imageWidth, u16 imageHeight)
{
	Material sand = materials[MaterialID::SAND]; // used as baseline.

	u16 scaledWidth = imageWidth / (scaleFactor * scaleFactor);
	u16 scaledHeight = imageHeight / (scaleFactor * scaleFactor);
	if (scaledWidth > cellWidth) {
		printf("Image Width too large! by: %d", scaledWidth - cellWidth);
		scaledWidth = cellWidth;
	}
	if (scaledHeight > cellHeight) {
		printf("Image Height too large! by: %d", scaledHeight - cellHeight);
		scaledHeight = cellHeight;
	}

	materials.push_back(Material(sand.r, sand.g, sand.b, sand.a, sand.d)); // create new material for image
	const u16 imgMatIdx = materials.size() - 1; // if there are more than 65536 materials, it deserves to die.
	Material& imgMat = materials[imgMatIdx];
	
	u32 texIdx = 0;
	u32 cellIdx = 0;
	for (s32 y = 0; y < scaledHeight; y++)
		for (s32 x = 0; x < scaledWidth; x++) {
			imgMat.variants.push_back({ 
					imageTextureData[texIdx + 0],
					imageTextureData[texIdx + 1],
					imageTextureData[texIdx + 2],
					imageTextureData[texIdx + 3]});
			cells[cellIdx].matID = imgMatIdx;
			cells[cellIdx].variant = imgMat.variants.size() - 1;

			texIdx += 4;
			cellIdx++;
		}
}
*/

/*
// slightly more challenging, can't set exact RGBA values, unless I create a new variant ? 
// create a new Material 'Image{ID}' ?
// Can't add a new index into MaterialID enum, not alterable at Runtime.
// Create a vector for Img Variants.
void Game::loadImage(std::vector<GLubyte> imageTextureData, u16 imageWidth, u16 imageHeight)
{
	Material sand = materials[MaterialID::SAND]; // used as baseline.

	u16 scaledWidth  = imageWidth  / (scaleFactor * scaleFactor);
	u16 scaledHeight = imageHeight / (scaleFactor * scaleFactor);
	if (scaledWidth  > cellWidth)  scaledWidth  = cellWidth;
	if (scaledHeight > cellHeight) scaledHeight = cellHeight;

	std::unordered_map<int, int> variantMap; // lets hope it initialises ints when idx created..
	auto hashRGBA = [](u8 r, u8 g, u8 b, u8 a) -> u32 { 
		return r * g * b * a % r+g+b+a; // simplest hash ever, slow though.. 
	};

	materials.push_back(Material(sand.r, sand.g, sand.b, sand.a, sand.d)); // create new material for image
	Material& imgMat = materials[materials.size() - 1];

	// puts the picture in top left of screen
	u32 idx = 0;
	for (s32 y = 0; y < scaledHeight; y++)
		for (s32 x = 0; x < scaledWidth; x++) {
			// for each new RGBA, if (copy)
			//	if copy -> use pre-existing variant idx
			//	else -> create new variant, use that
			//  
			// copy == hashRGBA(r,g,b,a)
			// 
			// R*G*B*A --> hash map --> profit?

			Cell& c = cells[cellIdx(x, y)];
			c.matID = materials.size() - 1; // set material to imgMat.

			// doesn't have any leeway for colours that are practically identical.
			// going to make more variants than neccessary.. oh well.
			if (variantMap[hashRGBA()] == 0) { // create new variant
				imgMat.variants.push_back({
					imageTextureData[idx + 0],
					imageTextureData[idx + 1], 
					imageTextureData[idx + 2], 
					imageTextureData[idx + 3]}); // initialising all these vectors is going to kill me..
				c.variant = imgMat.variants.size() - 1;
				variantMap[idx] = 100;
			} else // use pre-existing variant , perfection
				c.variant = variantMap[idx]; 

			idx += 4;
		}

	printf("Initialised Image, Variant size of: %d \n", imgMat.variants.size());
}
*/