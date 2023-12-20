#pragma once
#include "pch.h"
#include "cell.h"
#include "interfaceData.h"

class Game
{
public:
	Game();
	~Game();

	void init(std::vector<GLubyte>& texData, int textureW, int textureH, int scale);
	void reload(std::vector<GLubyte>& texData, int textureW, int textureH, int newScaleFactor);
	void loadImage(std::vector<GLubyte>& imageData, int imageW, int imageH);
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
	void circleFillAlgorithm(int x, int y, int radius, int CellTypeID, int range, int chance);
	void circleOutlineAlgorithm(int x, int y, int radius, int CellTypeID, int range);
	void squareAlgorithm(int x, int y, int radius, int CellTypeID, int range, int chance);
	void lineAlgorithm(int x, int y, int radius, int CellTypeID, int range, int chance);

	void updateSand(Cell& c);
	void updateWater(Cell& c);
	
	inline bool outOfBounds(int x, int y) { return (x >= cellW || x < 0 || y >= cellH || y < 0); }
	inline int cellIdx(int x, int y) { return (y * cellW) + x; }
	inline int texIdx(int x, int y) { return 4 * (y * texW + x); }
	inline std::vector<GLubyte> getTextureData() { return textureData; }
	//inline std::vector<GLubyte>* getTextureData_PTR() { return &textureData; }

	inline constexpr uint64_t splitMix64_NextRand()
	{
		uint64_t z = (x += UINT64_C(0x9E3779B97F4A7C15));
		z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
		z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
		return z ^ (z >> 31);
	}

	inline constexpr int64_t boundedRand(std::vector<int64_t> range) // slow as shit
	{ return range[splitMix64_NextRand() % range.size()]; }

	inline constexpr int randOffset()
	{ return randRange[splitMix64_NextRand() % 3]; } // hard coded. 

private:
	int randRange[3] = {-1, 0, 1};
	uint64_t x = 1234567890987654321; // Splitmix64 pseudo-rand algorithm.

	bool altCheck = false;
	const int g = 1;
	int texW, texH, cellScale, cellW, cellH;	

	std::vector<Cell> cells;
	std::vector<GLubyte> textureData;
	std::vector<CellType> Types;
	CellType EMPTY, SAND, WATER, CONCRETE, ALIVE;
	// change ^^ to enum ?? 
};