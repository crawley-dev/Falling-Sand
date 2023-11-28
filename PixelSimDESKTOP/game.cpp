#pragma once

#include "pch.h"
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

//	Types[0] = AIR	    = CellType(0, 188, 231, 255, 255, 0	);
//	Types[0] = AIR	    = CellType(0, 255, 255, 255, 255, 0	);
	
	Types.clear();
	EMPTY    = CellType(0,  44,  44,  44, 255,    0, 0); Types.push_back(EMPTY   );
	SAND	 = CellType(1, 245, 215, 176, 255, 1600, 4); Types.push_back(SAND    );
	WATER	 = CellType(2, 20 , 20 , 255, 125,  997, 4); Types.push_back(WATER   );
	CONCRETE = CellType(3, 200, 200, 200, 255, 2000, 4); Types.push_back(CONCRETE);
	ALIVE	 = CellType(4, 255,   0, 255, 255,    0, 0); Types.push_back(ALIVE   );

	cells.clear(); // once upon a time, it didn't reset cell arr on reload :/ oops
	cells.reserve(cellW * cellH); // memory leak if the function below is aborted before completion !!	

	// TODO: further testing, .reserve + [idx] vs push_back.
	// Could reserve ++ push_back_unchecked <-- doesn't do capcity checking, can break but also faster with proper reserve.
	for (int y = 0; y < cellH; ++y)
		for (int x = 0; x < cellW; ++x) { 
			cells.emplace_back(cellIdx(x, y), x, y, 5, EMPTY, false); //emplace_back doesn't create then copy an instance of the obj
			updatePixel(cells[cellIdx(x,y)]);
		}
}

void Game::reload(std::vector<GLubyte> newTexData, int newTexW, int newTexH, int newScaleFactor)
{
	int newCellW = newTexW / newScaleFactor;
	int newCellH = newTexH / newScaleFactor;
	std::vector<Cell> newCells;
	newCells.reserve(newCellW * newCellH);

	for (int y = 0; y < newCellH; ++y)
		for (int x = 0; x < newCellW; ++x) {
			if (outOfBounds(x, y))
				newCells.emplace_back(cellIdx(x, y), x, y, 1, EMPTY, false);
			else
				newCells.push_back(cells[cellIdx(x,y)]);
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
	cells.reserve(cellW * cellH);

	for (int y = 0; y < cellH; ++y)
		for (int x = 0; x < cellW; ++x) {
			cells.emplace_back(cellIdx(x, y), x, y, 5, EMPTY, false);
			updatePixel(cells[cellIdx(x,y)]);
		}
	resetSim = false;
}

// checkout quadtree approach.
void Game::update(interfaceData& data)
{
	if (texH == 0 || texW == 0) return;
	if (data.runSim && false) {
		mouseDraw(cellW, cellH, (data.frame % cellH) - 1, 100, 0, 1, 0);
		mouseDraw(cellW, cellH,       data.frame % cellH, 100, 1, 1, 0);
	}

	// use return cases they said.
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
			for (int x = 0; x < cellW - 1; x++) {
				Cell& c = cells[cellIdx(x, y)];
				cellUpdate(c);
				c.flag = false;
				updatePixel(c);
				//cellUpdate(c); // flickers without this.. EH
			}
	}

	data.frame++;
	//altCheck = !altCheck;
	//data.scanTopDown = !data.scanTopDown;
	
	/*
	else if (data.scanTopDown) {
		for (Cell& c : cells) {
			if (data.runSim) {
				//if (c.type.id == ALIVE.id) c.type.id = EMPTY.id;
				cellUpdate(c);
				c.flag = false; // to this day (25.9.23) i don't know why i can do this. 
			}
			updatePixel(c);
		}
	}
	else {
		for (int y = cellH - 1; y >= 0; y--)	
			for (int x = 0; x < cellW - 1; x++) {
				Cell& c = cells[cellIdx(x,y)];
				//if (c.type.id == ALIVE.id) c.type.id = EMPTY.id;

				if (data.runSim) {
					cellUpdate(c);
				}
				c.flag = false;
				updatePixel(c);
				if (data.runSim) cellUpdate(c); // flickers without this.. EH
			}
	}
	*/
}

// POSSIBLE IMPROVEMENTS
// Add 1 layer of padding, no need for out of bounds check 
// store states of all neighbour cells, only need to check 10% as many states
void Game::gameOfLifeUpdate(interfaceData& data)
{
#if true // the boring option... 
	std::vector<Cell> nextFrameCells = cells; // Copying all cells to new vec is current bottleneck
	for (int y = 1; y < cellH - 2; y++)	  // doesn't check edge cells ,means we unroll adjAlive for loo
		for (int x = 1; x < cellW - 2; x++) { // ++ gets rid of bounds checking if statement, 8 * cellW * cellH == lots of 'if'
			Cell& c = nextFrameCells[cellIdx(x, y)];
			//if (c.type.id != EMPTY.id || c.type.id != ALIVE.id) c.type.id = EMPTY.id;

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


			if (c.type.id == ALIVE.id) {
				if (adjAlive != 2 && adjAlive != 3) c.type = EMPTY;

				//if (adjAlive < 2) c.type = EMPTY;
				//else if (adjAlive >= 4) c.type = EMPTY;	
				else c.type = ALIVE;
			}
			else if (c.type.id == EMPTY.id)
				if (adjAlive == 3) 
					c.type = ALIVE;

			updatePixel(c);
		}
	cells = nextFrameCells; // Copying all cells is current bottleneck

#elif 1 // padding so I can remove a single IF statement. seems stupid..
		// questionable whether copying the ENTIRE cells vec just to avoid an if statement is optimal.
		// shouldn't need to copy all information stored in each cell. get it working then change.
	int paddedW = cellW + 1;
	int paddedH = cellH + 1;
	std::vector<Cell> newCells;

	newCells.reserve(paddedW * paddedH);

	// Map cells with padding.
	for (int y = 1; y < paddedH - 1; y++)	// start 1 deep and stop 1 short for padding.
		for (int x = 1; x < paddedW - 1; x++)	
			newCells[y * paddedH + x] = cells[cellIdx(x - 1, y - 1)];

	for (int x = 0; x < paddedW; x++) {					  // Horizontal Padding
		newCells[x].type = EMPTY;						  // top row		
		newCells[paddedW * paddedH - x - 1].type = EMPTY; // bottom row
	}
	for (int y = 0; y < paddedH; y++) {					  // Vertical Padding
		newCells[y * paddedW].type = EMPTY;				  // left column
		newCells[y * paddedW + paddedW - 1].type = EMPTY; // right column, should work ?
	}

	// Check within padding.
	for (int y = 1; y < cellH - 1; y++) 
		for (int x = 1; x < cellW - 1; x++) {
			Cell& c = newCells[cellIdx(x, y)];

			// unrolling the for loop, bit cheeky
			int adjAlive = 0;
			adjAlive += (newCells[cellIdx(x - 1, y - 1)].type.id == ALIVE.id); // TL
			adjAlive += (newCells[cellIdx(x + 0, y - 1)].type.id == ALIVE.id); // TM
			adjAlive += (newCells[cellIdx(x + 1, y - 1)].type.id == ALIVE.id); // TR
			adjAlive += (newCells[cellIdx(x - 1, y + 0)].type.id == ALIVE.id); // ML
			adjAlive += (newCells[cellIdx(x + 1, y + 0)].type.id == ALIVE.id); // MR
			adjAlive += (newCells[cellIdx(x - 1, y + 1)].type.id == ALIVE.id); // BL
			adjAlive += (newCells[cellIdx(x + 0, y + 1)].type.id == ALIVE.id); // BM
			adjAlive += (newCells[cellIdx(x + 1, y + 1)].type.id == ALIVE.id); // BR

			if (c.type.id == ALIVE.id) {
				if (adjAlive < 2) c.type = EMPTY;
				else if (adjAlive >= 4) c.type = EMPTY;
				else c.type = ALIVE;
			}
			else if (c.type.id == EMPTY.id)
				if (adjAlive == 3)
					c.type = ALIVE;

			updatePixel(c);
		}

#endif
}

#if 0
// uses a save state, storing all cells to be checked next generation
// PROBLEM: figuring out whether a certain cell has been added to iterate over next frame
//			having duplicates means its checked multiple times, messes up everything.
// uses padding to elimanate an if statement. minor tweak.
static std::vector<Cell> cellsToUpdate = cells; // initially check all cells.
std::vector<Cell> nextFrameAlive;
std::unordered_map<int, bool> umap;
//std::unordered_set<int> set; 
// could use an unordered_map<int, bool>
// first init all indexes of nextFrameAlive = false
// next: each time .push_back wants to be called, 
// first check the hash map index. very fast!
for (auto& c : nextFrameAlive) {
	std::pair<int, bool> temp(cellIdx(c.x, c.y), false);
	umap.insert(temp);
}

for (Cell& c : cellsToUpdate) {
	int adjAlive = 0;
	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++) {
			if (outOfBounds(c.x + i, c.y + j) || i == 0 && j == 0) continue; // could optimisze out.

			//if (set.find(cellIdx(c.x + i, c.y + j)) != set.end()) // check if added to hash map.
			//	nextFrameAlive.push_back(cells[cellIdx(c.x + i, c.y + j)]); // very slow though..
			try {
				if (!umap.at(cellIdx(c.x, c.y)))
					nextFrameAlive.push_back(cells[cellIdx(c.x + i, c.y + j)]);
			}
			catch (...) {}

			if (cells[cellIdx(c.x + i, c.y + j)].type.id == ALIVE.id)
				adjAlive++;
		}

	if (c.type.id == ALIVE.id) {
		if (adjAlive < 2) c.type = EMPTY;
		else if (adjAlive >= 4) c.type = EMPTY;
		else {
			c.type = ALIVE;
			nextFrameAlive.push_back(c);
		}
	}
	else if (c.type.id == EMPTY.id)
		if (adjAlive == 3) {
			c.type = ALIVE;
			nextFrameAlive.push_back(c);
		}
	updatePixel(c);
}
for (Cell& c : cellsToUpdate)
cells[c.x, c.y] = c;

cellsToUpdate = nextFrameAlive; // update cells to iter next frame

#endif

//void Game::gameOfLifeUpdateOptimised(interfaceData& data)

// for only 2 cases, switch is likely slower.
void Game::cellUpdate(Cell& c)
{
	if (c.flag) return;
	//switch (c.type.id) 
	//{
	//	case 1: updateSand(c); return;
	//	case 2: updateWater(c); return;
	//	default: return;
	//}
	if		(c.type.id == 1) updateSand(c);
	else if (c.type.id == 2) updateWater(c);
}

// dunno how to make this faster ... but its slow..
void Game::updatePixel(Cell& c)
{
	//if (outOfBounds(c.x, c.y)) return;
	for (int tY = 0; tY < cellScale; ++tY)
		for (int tX = 0; tX < cellScale; ++tX) {
			int idx = texIdx((c.x * cellScale) + tX, (c.y * cellScale) + tY);
			textureData[idx + 0] = c.type.r;
			textureData[idx + 1] = c.type.g;
			textureData[idx + 2] = c.type.b;
			textureData[idx + 3] = c.type.a;
		}
}

// this function is shit slow
CellType Game::varyPixelColour(int range, int cellTypeID)
{
	if (range <= 0) return Types[cellTypeID];
	CellType material = Types[cellTypeID];
	material.r = std::clamp(material.r - rand() % range, 0, 255);
	material.g = std::clamp(material.g - rand() % range, 0, 255);
	material.b = std::clamp(material.b - rand() % range, 0, 255);
	material.a = std::clamp(material.a - rand() % range, 0, 255);
	return material;
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

void Game::mouseDraw(int mx, int my, int radius, int chance, int CellTypeID, int CellDrawShape, int range)
{
	const int x = mx / cellScale;
	const int y = my / cellScale;

	// edge cases.
	if (outOfBounds(x, y)) 
		return;
	else if (radius == 1)
		changeCellType(x, y, CellTypeID, range);

	// looks cleaner.
	// quite a few parameters for the functions though..
	switch (CellDrawShape) {
		case 0:
			circleFillAlgorithm(x, y, radius, CellTypeID, range, chance); return;
		case 1:															  
			circleOutlineAlgorithm(x, y, radius, CellTypeID, range); return;
		case 2:															  
			lineAlgorithm(x, y, radius, CellTypeID, range, chance); return;
		case 3:															  
			squareAlgorithm(x, y, radius, CellTypeID, range, chance); return;
	}
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

// TODO: update velocity increase to deltaTime based.
//		i.e: velocity += accel * deltaTime <-- actually increasing acceleration with time
// Current rand is impacting performance. gpu pseudo rand? 
// can't send each rand to gpu each time, 
// would need to group them together in 1 buffer
void Game::updateSand(Cell& c)
{
#if 1
	// this looks abhorrent
	if (checkDensity(c, 0, 1)) return;
	if (altCheck) {
		if (checkDensity(c,  1, 1)) return;
		if (checkDensity(c, -1, 1)) return;
	}					 
	else {				 
		if (checkDensity(c, -1, 1)) return;
		if (checkDensity(c,  1, 1)) return;
	}
#else 
	//c.v = 1; // didnt move so reset velocity. 
	//++c.v = std::clamp(c.v, 0, c.type.tV); // this looks proper autistic
#endif
}
 
void Game::updateWater(Cell& c)
{
	if		(checkDensity(c,  0, 1)) return;
	else if (checkDensity(c, -1, 1)) return;
	else if (checkDensity(c,  1, 1)) return;
	else if (checkDensity(c, -1, 0)) return;
	else if (checkDensity(c,  1, 0)) return;
}