#pragma once
#include "pch.h"
#include "cell.h"
#include "interfaceData.h"

class Game
{
public:
	Game();
	~Game();


	void init(u16 newTextureWidth, u16 newTextureHeight, u8 newScaleFactor);
	void reload(u16 newTextureWidth, u16 newTextureHeight, u8 newScaleFactor);
	//void loadImage();
	void reset();

	void updateSim(interfaceData& iData);
	void topDown_Update();
	void bottomUp_Update();
	void snake_Update();
	void gol_Update();

	// passing around more data than last time lol, u32 ptr && more.
	void changeMaterial(u16 x, u16 y, u8 newMaterial);
	void swapCells(Cell& c1, Cell& c2);
	bool trySwap(Cell& c1, u16 x2, u16 y2);

	void updateCell(Cell& c, u16 x, u16 y);
	void updateSand(Cell& c, u16 x, u16 y);
	void updateWater(Cell& c, u16 x, u16 y);
	void updateTextureData(std::vector<GLubyte>& textureData, u32 width);
	//std::vector<GLubyte> updateTextureData();

	void mouseDraw(u16 x, u16 y, u16 size, u8 drawChance, u8 material, u8 shape);
	void draw_Circle(u16 x, u16 y, u16 size, u8 material, u8 drawChance);
	void draw_CircleSegments(u16 xc, u16 yc, u16 x, u16 y, u8 material);
	void draw_CircleOutline(u16 x, u16 y, u16 size, u8 material, u8 drawChance);
	void draw_Line(u16 x, u16 y, u16 size, u8 material, u8 drawChance);
	void draw_Square(u16 x, u16 y, u16 size, u8 material, u8 drawChance);

	inline u32 cellIdx(u16 x, u16 y) { return (y * cellWidth) + x; }	
	inline u32 textureIdx(u16 x, u16 y) { return 4 * ((y * textureWidth) + x); }
	inline bool outOfBounds(u16 x, u16 y) 									 
	{ return x >= cellWidth || y >= cellHeight || x < 0 || y < 0; }

	inline u64 splitMix64_NextRand()
	{
		u64 z = (x += UINT64_C(0x9E3779B97F4A7C15));
		z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
		z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
		return z ^ (z >> 31);
	}

	template<typename T> // cheeky template
	inline T getRand(T min = -1, T max = 1) 
	{ return splitMix64_NextRand() % (max - min + 1) + min; }

private:
	u64 x = 1234567890987654321; // Splitmix64 seed.
	
	u8 nVariants;
	u8 scaleFactor;
	u16 textureWidth, textureHeight;
    u16 cellWidth, cellHeight;
	std::vector<Cell> cells;
	std::vector<Material> materials;
};
