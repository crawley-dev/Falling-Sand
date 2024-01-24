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
	u8 variation = 8; // don't raise this too high, will over/underflow u8..
	for (Material& mat : materials) {
		mat.variants.clear();
		mat.variants.reserve(nVariants);
		for (u8 i = 0; i < nVariants; i++) {
			std::vector<u8> variant;

			if (mat.d == 1600 || mat.d == 997) {
				//if (mat.r < 128) variant.push_back(mat.r - getRand<u8>(0, variation));
				//else variant.push_back(mat.r + getRand<u8>(0, variation));
				//
				//if (mat.r < 128) variant.push_back(mat.g - getRand<u8>(0, variation));
				//else variant.push_back(mat.g + getRand<u8>(0, variation));
				//
				//if (mat.r < 128) variant.push_back(mat.b - getRand<u8>(0, variation));
				//else variant.push_back(mat.b + getRand<u8>(0, variation));
				variant.push_back(mat.r - getRand<u8>(0, variation));
				variant.push_back(mat.g - getRand<u8>(0, variation));
				variant.push_back(mat.b - getRand<u8>(0, variation));
				variant.push_back(mat.a); // dont mess with opacity..
			}
			else {
				variant.push_back(mat.r);
				variant.push_back(mat.g);
				variant.push_back(mat.b);
				variant.push_back(mat.a);
			}

			mat.variants.push_back(variant);
		}
	}

	cells.clear();
	cells.reserve(cellWidth * cellHeight);
	for (s32 i = 0; i < cellWidth * cellHeight; i++) // init cell.updated = true so updateTextureData runs on first time.
		cells.emplace_back(MaterialID::EMPTY, true, getRand<u8>(0, nVariants - 1), 0);
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
				newCells.emplace_back(MaterialID::EMPTY, false, getRand<u8>(0, nVariants - 1), 0);
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
		cells.emplace_back(MaterialID::EMPTY, false, getRand<u8>(0, nVariants - 1), 0);
	sizeChanged = true;
}

/*--------------------------------------------------------------------------------------
---- Simulation Update Routines --------------------------------------------------------
--------------------------------------------------------------------------------------*/

void Game::updateSim(interfaceData& data)
{	// keep some form of global index that is incremented with each cell
	// thereby eliminating the need to pass x,y to cellUpdate etc. instead just cell.

	if (!data.runSim) return;
	
	fluidDispersionFactor = data.fluidDispersionFactor;
	solidDispersionFactor = data.solidDispersionFactor;

	switch (data.scanMode) {
	case Scan::TOP_DOWN:		topDown_Update();	 break;
	case Scan::BOTTOM_UP_LEFT:	l_bottomUp_Update(); break;
	case Scan::BOTTOM_UP_RIGHT:	r_bottomUp_Update(); break;
	case Scan::SNAKE:			snake_Update();		 break;
	case Scan::GAME_OF_LIFE:	gol_Update();		 break;
	}

	//if (data.updateMode == Update::FLICKER) {
	//	if (data.scanMode == Scan::TOP_DOWN)
	//		data.scanMode = Scan::BOTTOM_UP_LEFT;
	//	else if (data.scanMode == Scan::BOTTOM_UP_LEFT && data.frame % 3 == 0)
	//		data.scanMode = Scan::TOP_DOWN;
	//}

	if (data.updateMode == Update::FLICKER) {
		if (data.scanMode == Scan::BOTTOM_UP_LEFT)
			data.scanMode = Scan::BOTTOM_UP_RIGHT;
		else
			data.scanMode = Scan::BOTTOM_UP_LEFT;
	}

	data.textureChanges = textureChanges.size();
	data.frame++;
}

void Game::topDown_Update()
{
	for (s32 y = 0; y < cellHeight; y++)
		for (s32 x = 0; x < cellWidth; x++) {
			Cell& c = cells[cellIdx(x, y)];
			updateCell(c, x, y);
		}
}

void Game::l_bottomUp_Update()
{
	for (s32 y = cellHeight - 1; y >= 0; y--)
		for (s32 x = 0; x < cellWidth; x++) {
			Cell& c = cells[cellIdx(x, y)];
			updateCell(c, x, y);
		}
}

void Game::r_bottomUp_Update() {
	for (s32 y = cellHeight - 1; y >= 0; y--)
		for (s32 x = cellWidth - 1; x >= 0; x--) {
			Cell& c = cells[cellIdx(x, y)];
			updateCell(c, x, y);
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
			}
		else					 // <--
			for (s32 x = cellWidth - 1; x >= 0; x--) {
				Cell& c = cells[cellIdx(x, y)];
				updateCell(c, x, y);
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
	case MaterialID::SAND:	updateSand(x, y);  break;
	case MaterialID::WATER: updateWater(x, y); break;
	}
}

#define EUCLIDEAN 1
#define EUCLIDEAN_Y 1
#if EUCLIDEAN

void Game::updateSand(u16 x, u16 y)
{
	// Priority:
	// 1. Down
	// 2. Diagonals (random which one is higher priority)
	Cell& c = cells[cellIdx(x, y)];

	s8 yDispersion = 1; // don't check for yDispersion = 0, only check vertical neighbours
	s8 xDispersion = 0;

	// calc yDispersion
	for (u8 dY = 1; dY <= solidDispersionFactor; dY++) {
		if (!querySwap(x, y, x, y + dY)) break;
		yDispersion = dY;
	}

	// check if all 'moves' have been used.
	if (yDispersion >= solidDispersionFactor) {
		trySwap(x, y, x, y + yDispersion);
		return;
	}

	// whilst (mag(dX) + mag(dY) <= solidDispersionFactor)
	// 1.  check for a side to side movement. << rand
	// 2.	if possible: set side to side movement to new xDispersion
	//		else: break;
	// 3.	check vertical movement  (x+xDispersion, y+yDispersion)
	// 4.	if possible: next iteration
	//		else: break;

	while (abs(xDispersion) + abs(yDispersion) <= solidDispersionFactor) {
		s8 dX = getRand<s8>();
		if (!querySwap(x, y, x + dX, y + yDispersion)) break; // check if side to side movement is possible
		xDispersion = dX;
		if (!querySwap(x, y, x + xDispersion, y + yDispersion + 1)) break; // check if vertical movement is possible
		yDispersion++;
	}

	trySwap(x, y, x + xDispersion, y + yDispersion);
}

// doesn't actually check if the cell is occupied.
void Game::updateWater(u16 x, u16 y) {
	s8 yDispersion = 0;
	s8 xDispersion = 0;

	// First calculate vertical movement
	for (u8 dY = 1; dY <= fluidDispersionFactor; dY++) {
		if (!querySwap(x, y, x, y + dY)) break;
		yDispersion = dY;
	}

	// check if all 'moves' have been used.
	if (yDispersion >= fluidDispersionFactor) {
		trySwap(x, y, x, y + yDispersion);
		return;
	}

	// calculate horizontal movement
	for (u8 dX = 1; dX <= fluidDispersionFactor - yDispersion; dX++) {
		if (getRand<s8>(1, 100) > 50) { // "more random" than 0 || 1
			if (querySwap(x, y, x + dX, y)) xDispersion = dX;
			else if (querySwap(x, y, x - dX, y)) xDispersion = -dX;
			else break;
		}
		else {
			if (querySwap(x, y, x - dX, y)) xDispersion = -dX;
			else if (querySwap(x, y, x + dX, y)) xDispersion = dX;
			else break;
		}
	}

	trySwap(x, y, x + xDispersion, y + yDispersion);
}

bool Game::trySwap(u16 x1, u16 y1, u16 x2, u16 y2)
{
	if (outOfBounds(x1, y1) || outOfBounds(x2, y2)) return false;

	Cell& c1 = cells[cellIdx(x1, y1)];
	Cell& c2 = cells[cellIdx(x2, y2)];
	if (materials[c1.matID].d <= materials[c2.matID].d) return false;

	swapCells(x1,y1,x2,y2);
	return true;
}

bool Game::querySwap(u16 x1, u16 y1, u16 x2, u16 y2)
{
	if (outOfBounds(x1, y1) || outOfBounds(x2, y2)) return false;

	Cell& c1 = cells[cellIdx(x1, y1)];
	Cell& c2 = cells[cellIdx(x2, y2)];
	if (materials[c1.matID].d <= materials[c2.matID].d) return false;
	
	return true;
}

void Game::changeMaterial(u16 x, u16 y, u8 newMaterial)
{
	if (outOfBounds(x, y)) return; // not consistent control flow, but it works.
	Cell& c = cells[cellIdx(x, y)];
	c.matID = newMaterial;
	c.updated = true;

	textureChanges.push_back(std::pair<u16,u16>(x,y));
}

void Game::swapCells(u16 x1, u16 y1, u16 x2, u16 y2)
{
	Cell& c1 = cells[cellIdx(x1, y1)];
	Cell& c2 = cells[cellIdx(x2, y2)];

	u8 temp_MaterialID = c1.matID;
	c1.matID = c2.matID;
	c2.matID = temp_MaterialID;

	c1.updated = true;
	c2.updated = true;

	textureChanges.push_back(std::pair<u16, u16>(x1, y1));
	textureChanges.push_back(std::pair<u16, u16>(x2, y2));
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

// this causes flickering.. switching back to entire texture update.
// might not be calling all cells that need updating, check cellswap etc.
void Game::updateTextureData(std::vector<GLubyte>& textureData)
{
	if (sizeChanged) {
		updateEntireTextureData(textureData);
		sizeChanged = false;
		return;
	}

	for (const auto& [x,y] : textureChanges) { // cheeky structured binding.
		Cell& c = cells[cellIdx(x,y)];
		const std::vector<u8>& variant = materials[c.matID].variants[c.variant];
 		
		const GLubyte red = variant[0];
		const GLubyte green = variant[1];
		const GLubyte blue = variant[2];
		const GLubyte alpha = variant[3];
		
		for (s32 tY = 0; tY < scaleFactor; tY++)
			for (s32 tX = 0; tX < scaleFactor; tX++) {
				s32 texIdx = textureIdx((x * scaleFactor) + tX, (y * scaleFactor) + tY);
				textureData[texIdx + 0] = red;
				textureData[texIdx + 1] = green;
				textureData[texIdx + 2] = blue;
				textureData[texIdx + 3] = alpha;
			}
		c.updated = false;
	}
	
	textureChanges.clear();
}

void Game::updateEntireTextureData(std::vector<GLubyte>& textureData)
{
	for (s32 y = 0; y < cellHeight; y++) {
		for (s32 x = 0; x < cellWidth; x++) {
			Cell& c = cells[cellIdx(x, y)];
			const std::vector<u8>& variant = materials[c.matID].variants[c.variant];
			
			const GLubyte red = variant[0];
			const GLubyte green = variant[1];
			const GLubyte blue = variant[2];
			const GLubyte alpha = variant[3];

			for (s32 tY = 0; tY < scaleFactor; tY++)
				for (s32 tX = 0; tX < scaleFactor; tX++) {
					s32 idx = textureIdx((x * scaleFactor) + tX, (y * scaleFactor) + tY);
					textureData[idx + 0] = red;
					textureData[idx + 1] = green;
					textureData[idx + 2] = blue;
					textureData[idx + 3] = alpha;
				}
			c.updated = false;
		}
	}
}

void Game::loadImage(std::vector<GLubyte> imageTextureData, u16 imageWidth, u16 imageHeight)
{
	// ?? scale imageWidth and imageHeight to cellWidth and cellHeight 
	//		^^ only really necessary for low res // really high res
	// .. interpolate image RGBA --> closest material RGBA
	// .. .. avoids 100k variant vector stuff.
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


void Game::updateSand(Cell& c, u16 x, u16 y)
{
	//if (trySwap(c, x, y + 1)) return;
	//if (trySwap(c, x + getRand<s8>(), y + 1)) return;

	s8 yDispersion = 0;
	s8 xDispersion = 0;
	//for (u8 dY = 1; dY < solidDispersionFactor; dY++) {
	//	if (outOfBounds(x, y + dY)) goto SAND_DISPERSE_GOTO;
	//	yDispersion = dY;
	//}

	for (u8 dY = 1; dY <= solidDispersionFactor; dY++) {
		for (s8 dX = -solidDispersionFactor; dX <= solidDispersionFactor; dX++) {
			//changeMaterial(x + dX, y + dY, MaterialID::CONCRETE);

			if (outOfBounds(x + dX, y + dY)) goto SAND_DISPERSE_GOTO;
			else xDispersion = dX;

			if (outOfBounds(x - dX, y + dY)) goto SAND_DISPERSE_GOTO;
			else xDispersion = -dX;
		}
		yDispersion++;
	}

	// ideally want to use bottom half of a semi-circle, velocity == radius.. GOOD!
	// at init() could hard code offset values in a vector for circumference of the circle.
	// ^^ wouldn't really work for a 2d vector velocity system..
	// what do I even use to calculate x velocity of a cell? weird..


SAND_DISPERSE_GOTO:
	//printf("Dispersed: %d, %d\n", (int)xDispersion, (int)yDispersion);
	trySwap(c, x + xDispersion, y + yDispersion);
}

*/
